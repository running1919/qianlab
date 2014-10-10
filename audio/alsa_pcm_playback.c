#include <stdio.h>

#include <alsa/asoundlib.h>
#include <mad.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

/*
 *sample_rate * format(quantization bits) * channel_nums / 8 = bytes_per_second;
 * sample_length = 8 or 16
 * channel = 1 or 2
 * frame = sample_length *channel
 * period = how mang frames
 *
 * compile method:
 *                gcc alsa_pcm_playback.c -o appb -lasound -lmad
 * using method:
 *              ./appb              or
 *              ./app [media filepath]
 */
static void set_pcm_hw_params(snd_pcm_t *handle, snd_pcm_hw_params_t *params,
								unsigned int sample_rate, snd_pcm_uframes_t frames, int dir);

static int simple_urandom_nosie();
static int simple_play_music(char *media_path);

snd_pcm_t *handle;
snd_pcm_hw_params_t *params;

int main(int argc, char *argv[])
{
	int ret;
	unsigned int sample_rate;
	int dir = 0;
	snd_pcm_uframes_t frames;

	ret = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (ret < 0) {
		fprintf(stderr, "unable to open pcm device: %s\n",
			snd_strerror(ret));
	}
//	printf("PCM handle name = '%s'\n", snd_pcm_name(handle));

	snd_pcm_hw_params_alloca(&params);//this func must be call native ,why? TO DO...

	/******setup pcm hw params*******/
	sample_rate = 48000;//just i want
	frames = 32;//just i want
	set_pcm_hw_params(handle, params, sample_rate, frames, dir);

	if (argc < 2) {
		printf("nosie test...\n\n");
		simple_urandom_nosie();
	}
	else {
		printf("play music test...\n\n");
		simple_play_music(argv[1]);
	}

	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	return 0;
}

static void set_pcm_hw_params(snd_pcm_t *handle, snd_pcm_hw_params_t *params,
							  unsigned int sample_rate, snd_pcm_uframes_t frames, int dir)
{
	//snd_pcm_t *handle;
	//snd_pcm_hw_params_t *params;
	int ret;

	snd_pcm_hw_params_any(handle, params);
	snd_pcm_hw_params_set_access(handle, params,
				SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(handle, params,
				SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(handle, params, 2);

	snd_pcm_hw_params_set_rate_near(handle, params,
				&sample_rate, &dir);

	snd_pcm_hw_params_set_period_size_near(handle,
				params, &frames, &dir);

	ret = snd_pcm_hw_params(handle, params);
	if (ret < 0) {
		fprintf(stderr, "unable to set hw params: %s\n",
			snd_strerror(ret));
	}
}


struct buffer {
	unsigned char const *start;
	unsigned long length;
};

static enum mad_flow input(void *data, struct mad_stream *stream)
{
	struct buffer *buffer = data;

	printf("this is input\n");
	if (!buffer->length)
	return MAD_FLOW_STOP;

	mad_stream_buffer(stream, buffer->start, buffer->length);

	buffer->length = 0;

return MAD_FLOW_CONTINUE;
}

static inline signed int scale(mad_fixed_t sample)
{
	/* round */
	sample += (1L << (MAD_F_FRACBITS - 16));

	/* clip */
	if (sample >= MAD_F_ONE)
	sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
	sample = -MAD_F_ONE;

	/* quantize */
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}

static char *output_buffer = NULL;
static unsigned int already_malloc = 0;
static int pre_frames;

static enum mad_flow output(void *data,
					 struct mad_header const *header,
					 struct mad_pcm *pcm)
{
	unsigned int nchannels, cur_frames;
	mad_fixed_t const *left_ch, *right_ch;
	char *dest;
	int ret;

/* pcm->samplerate contains the sampling frequency */
	nchannels = pcm->channels;
	cur_frames  = pcm->length;
	left_ch   = pcm->samples[0];
	right_ch  = pcm->samples[1];

	if (already_malloc == 0) {
		printf("the bitrate is %d kbps %d\n", header->bitrate / 1000);
		printf("malloc %d bytes space\n", cur_frames * nchannels * sizeof(short));//short is 16 bits,SND_PCM_FORMAT_S16_LE
		output_buffer = (char *)malloc(cur_frames * nchannels * sizeof(short));

		already_malloc = 1;
		pre_frames = cur_frames;
	}
	else if (cur_frames > pre_frames) {
		free(output_buffer);

		printf("remalloc %d bytes space\n", cur_frames * nchannels * sizeof(short));//short is 16 bits
		output_buffer = (char *)malloc(cur_frames * nchannels * sizeof(short));

		pre_frames = cur_frames;
	}

	dest = output_buffer;

	while (cur_frames--) {
		signed int sample;

		/* output sample(s) in 16-bit signed little-endian PCM */
		sample = scale(*left_ch++);
		*dest++ = ((sample >> 0) & 0xff);
		*dest++ = ((sample >> 8) & 0xff);

		if (nchannels == 2) {
			sample = scale(*right_ch++);
			*dest++ = ((sample >> 0) & 0xff);
			*dest++ = ((sample >> 8) & 0xff);
		}
	}

    snd_pcm_uframes_t frames = pcm->length;

	ret = snd_pcm_writei(handle, output_buffer, frames);
	if (ret == -EPIPE) {
		/* EPIPE means underrun */
		fprintf(stderr, "underrun occurred\n");
		snd_pcm_prepare(handle);
	}
	else if (ret < 0) {
		fprintf(stderr,"error from writei: %s\n",
			snd_strerror(ret));
	}
	else if (ret != (int)frames) {
		fprintf(stderr, "short write, write %d frames\n", ret);
	}

//	free(buffer);

	return MAD_FLOW_CONTINUE;

}

static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
	struct buffer *buffer = data;
	fprintf(stderr, "error: decoding error 0x%04x (%s) at byte offset %u\n",
			stream->error, mad_stream_errorstr(stream),
			stream->this_frame - buffer->start);

	/* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */
	return MAD_FLOW_CONTINUE;
}

static int decode(unsigned char const *start, unsigned long length)
{
	struct buffer buffer;
	struct mad_decoder decoder;
	int result;

	/* initialize our private message structure */
	buffer.start  = start;
	buffer.length = length;

	/* configure input, output, and error functions */
	mad_decoder_init(&decoder, &buffer,
					 input, 0 /* header */, 0 /* filter */, output,
					 error, 0 /* message */);

	/* start decoding */
	printf("starting decoding...\n");
	result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

	/* release the decoder */
	mad_decoder_finish(&decoder);

	if (output_buffer != NULL)
		free(output_buffer);

	return result;
}

static int simple_play_music(char *media_path)
{
	struct stat stat;

	int fd = open(media_path, O_RDWR);
	if (fd < 0) {
		perror("open file failed");
		return fd;
	}

	if (fstat(fd, &stat) == -1 ||stat.st_size == 0) {
		printf("fstat failed:\n");
		return -1;
	}

	void *fdm = mmap(0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (fdm == MAP_FAILED)
		return (int)MAP_FAILED;

	decode(fdm, stat.st_size);

	if (munmap(fdm, stat.st_size) == -1)
		return -1;

	close(fd);
}

static int simple_urandom_nosie()
{
	snd_pcm_uframes_t frames;
	int dir, fd, ret;
	unsigned int val;
	char *buffer;
	long loops;
	int period_size;

	snd_pcm_hw_params_get_period_size(params, &frames, &dir);//get real frames
	period_size = frames * 4; /// 2 bytes/sample, 2 channels

	buffer = (char *)malloc(period_size);

	snd_pcm_hw_params_get_period_time(params, &val, &dir);
	loops = 5000000 / val;
	printf("5 seconds need  %d loops\n", loops);

	fd = open("/dev/urandom", O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "open /dev/urandom failed\n");
		return fd;
	}

	while (loops > 0) {
		loops--;
		ret = read(fd, buffer, period_size);

		if (ret == 0) {
			fprintf(stderr, "end of file on input\n");
			break;
		}
		else if (ret != period_size) {
			fprintf(stderr,
				"short read: read %d bytes\n", ret);
		}

		 ret = snd_pcm_writei(handle, buffer, frames);

		if (ret == -EPIPE) {
			/* EPIPE means underrun */
			fprintf(stderr, "underrun occurred\n");
			snd_pcm_prepare(handle);
		}
		else if (ret < 0) {
			fprintf(stderr,"error from writei: %s\n",
				snd_strerror(ret));
		}
		else if (ret != (int)frames) {
			fprintf(stderr, "short write, write %d frames\n", ret);
		}
	}
	close(fd);

	return 0;
}




