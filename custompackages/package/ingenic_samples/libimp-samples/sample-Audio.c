/*
 * sample-audio.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <imp/imp_audio.h>
#include <imp/imp_log.h>

#define TAG "Sample-Audio"

#define AEC_SAMPLE_RATE 8000
#define AEC_SAMPLE_TIME 10

#define IMP_AUDIO_BUF_SIZE (5 * (AEC_SAMPLE_RATE * sizeof(short) * AEC_SAMPLE_TIME / 1000))
#define IMP_AUDIO_RECORD_NUM 200

#define IMP_AUDIO_RECORD_ALGO_AI_FILE "/tmp/record_algo_ai_file.pcm"
#define IMP_AUDIO_RECORD_VOLTEST_FILE "/tmp/record_voltest_file.pcm"
#define IMP_AUDIO_RECORD_FILE "/tmp/record_file.pcm"
#define IMP_AUDIO_ENCODE_FILE "/tmp/record_file.g711"
#define IMP_AUDIO_PLAY_FILE "/tmp/play_file.pcm"
#define IMP_AUDIO_FILTER_FILE "/tmp/filter_file.pcm"

/* My G711 Encoder */
#define SIGN_BIT    (0x80)      /* Sign bit for a A-law byte. */
#define QUANT_MASK  (0xf)       /* Quantization field mask. */
#define NSEGS       (8)         /* Number of A-law segments. */
#define SEG_SHIFT   (4)         /* Left shift for segment number. */
#define SEG_MASK    (0x70)      /* Segment field mask. */
#define BIAS        (0x84)      /* Bias for linear code. */

static short seg_end[8] = {0xFF, 0x1FF, 0x3FF, 0x7FF,
	0xFFF, 0x1FFF, 0x3FFF, 0x7FFF};

static int search(int val, short	*table, int	size)
{
	int	i;

	for (i = 0; i < size; i++) {
		if (val <= *table++)
			return (i);
	}
	return (size);
}

static int alaw2linear( unsigned char a_val )
{
	int	t;
	int	seg;

	a_val ^= 0x55;

	t = (a_val & QUANT_MASK) << 4;
	seg = ( (unsigned)a_val & SEG_MASK ) >> SEG_SHIFT;
	switch (seg)
	{
		case 0:
			t += 8;
			break;
		case 1:
			t += 0x108;
			break;
		default:
			t += 0x108;
			t <<= seg - 1;
	}
	return ((a_val & SIGN_BIT) ? t : -t);
}

static int ulaw2linear(unsigned char u_val)
{
	int	t;

	u_val = ~u_val;

	t = ((u_val & QUANT_MASK) << 3) + BIAS;
	t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;

	return ((u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
}

static unsigned char linear2alaw(int pcm_val)
{
	int		mask;
	int		seg;
	unsigned char	aval;

	if (pcm_val >= 0) {
		mask = 0xD5;		/* sign (7th) bit = 1 */
	} else {
		mask = 0x55;		/* sign bit = 0 */
		pcm_val = -pcm_val - 8;
	}

	seg = search(pcm_val, seg_end, 8);
	if (seg >= 8)		/* out of range, return maximum value. */
		return (0x7F ^ mask);
	else {
		aval = seg << SEG_SHIFT;
		if (seg < 2)
			aval |= (pcm_val >> 4) & QUANT_MASK;
		else
			aval |= (pcm_val >> (seg + 3)) & QUANT_MASK;
		return (aval ^ mask);
	}
}

static unsigned char linear2ulaw(int pcm_val)
{
	int		mask;
	int		seg;
	unsigned char	uval;

	if (pcm_val < 0) {
		pcm_val = BIAS - pcm_val;
		mask = 0x7F;
	} else {
		pcm_val += BIAS;
		mask = 0xFF;
	}

	seg = search(pcm_val, seg_end, 8);
	if (seg >= 8)		/* out of range, return maximum value. */
		return (0x7F ^ mask);
	else {
		uval = (seg << 4) | ((pcm_val >> (seg + 3)) & 0xF);
		return (uval ^ mask);
	}
}

static int my_g711a_decode( short amp[], const unsigned char g711a_data[], int g711a_bytes )
{
	int i;
	int samples;
	unsigned char code;
	int sl;

	for ( samples = i = 0; ; )
	{
		if (i >= g711a_bytes)
			break;
		code = g711a_data[i++];

		sl = alaw2linear( code );

		amp[samples++] = (short) sl;
	}
	return samples*2;
}

static int my_g711u_decode(short amp[], const unsigned char g711u_data[], int g711u_bytes)
{
	int i;
	int samples;
	unsigned char code;
	int sl;

	for (samples = i = 0;;)
	{
		if (i >= g711u_bytes)
			break;
		code = g711u_data[i++];

		sl = ulaw2linear(code);

		amp[samples++] = (short) sl;
	}
	return samples*2;
}

static int my_g711a_encode(unsigned char g711_data[], const short amp[], int len)
{
	int i;

	for (i = 0;  i < len;  i++) {
		g711_data[i] = linear2alaw(amp[i]);
	}

	return len;
}

static int my_g711u_encode(unsigned char g711_data[], const short amp[], int len)
{
	int i;

	for (i = 0;  i < len;  i++) {
		g711_data[i] = linear2ulaw(amp[i]);
	}

	return len;
}

static int MY_Audio_PCM2G711a(char *InAudioData, char *OutAudioData, int DataLen)
{
	int Retaen = 0;

	if((NULL == InAudioData) && (NULL == OutAudioData) && (0 == DataLen)) {
		IMP_LOG_ERR(TAG, "Error, empty data or transmit failed, exit !\n");
		return -1;
	}

	Retaen = my_g711a_encode((unsigned char *)OutAudioData, (short*)InAudioData, DataLen/2);

	return Retaen;
}

static int MY_Audio_PCM2G711u(char *InAudioData, char *OutAudioData, int DataLen)
{
	int Retuen = 0;

	if((NULL == InAudioData) && (NULL == OutAudioData) && (0 == DataLen)) {
		IMP_LOG_ERR(TAG, "Error, empty data or transmit failed, exit !\n");
		return -1;
	}

	Retuen = my_g711u_encode((unsigned char *)OutAudioData, (short*)InAudioData, DataLen/2);

	return Retuen;
}

static int MY_Audio_G711a2PCM(char *InAudioData, char *OutAudioData, int DataLen)
{
	int Retade = 0;

	if((NULL == InAudioData) && (NULL == OutAudioData) && (0 == DataLen)) {
		IMP_LOG_ERR(TAG, "Error, empty data or transmit failed, exit !\n");
		return -1;
	}

	Retade = my_g711a_decode((short*)OutAudioData, (unsigned char *)InAudioData, DataLen);

	return Retade;
}

static int MY_Audio_G711u2PCM(char *InAudioData, char *OutAudioData, int DataLen)
{
	int Retude = 0;

	if((NULL == InAudioData) && (NULL == OutAudioData) && (0 == DataLen)) {
		IMP_LOG_ERR(TAG, "Error, empty data or transmit failed, exit !\n");
		return -1;
	}

	Retude = my_g711u_decode((short*)OutAudioData, (unsigned char *)InAudioData, DataLen);

	return Retude;
}

static int MY_G711A_Encode_Frm(void *encoder, IMPAudioFrame *data, unsigned char *outbuf,int *outLen)
{
	*outLen = MY_Audio_PCM2G711a((char *)data->virAddr, (char *)outbuf, data->len);
	return 0;
}

static int MY_G711U_Encode_Frm(void *encoder, IMPAudioFrame *data, unsigned char *outbuf,int *outLen)
{
	*outLen = MY_Audio_PCM2G711u((char *)data->virAddr, (char *)outbuf, data->len);
	return 0;
}

static int MY_G711A_Decode_Frm(void *decoder, unsigned char
		*inbuf,int inLen, unsigned short *outbuf,int
		*outLen,int *chns)
{
	*outLen = MY_Audio_G711a2PCM((char *)inbuf, (char *)outbuf, inLen);
	return 0;
}

static int MY_G711U_Decode_Frm(void *decoder, unsigned char
		*inbuf,int inLen, unsigned short *outbuf,int
		*outLen,int *chns)
{
	*outLen = MY_Audio_G711u2PCM((char *)inbuf, (char *)outbuf, inLen);
	return 0;
}

/* Test API */
static int IMP_Audio_Encode(void)
{
	char *buf_pcm = NULL;
	int ret = -1;

	buf_pcm = (char *)malloc(IMP_AUDIO_BUF_SIZE);
	if(buf_pcm == NULL) {
		IMP_LOG_ERR(TAG, "malloc audio pcm buf error\n");
		return -1;
	}

	FILE *file_pcm = fopen(IMP_AUDIO_RECORD_FILE, "rb");
	if(file_pcm == NULL) {
		IMP_LOG_ERR(TAG, "fopen %s failed\n", IMP_AUDIO_RECORD_FILE);
		return -1;
	}

	FILE *file_g711 = fopen(IMP_AUDIO_ENCODE_FILE, "wb");
	if(file_g711 == NULL) {
		IMP_LOG_ERR(TAG, "fopen %s failed\n", IMP_AUDIO_ENCODE_FILE);
		return -1;
	}

	/* my G711A encoder Register */
	int handle_g711a = 0;
	IMPAudioEncEncoder my_encoder;
	my_encoder.maxFrmLen = 1024;
	sprintf(my_encoder.name, "%s", "MY_G711A");
	my_encoder.openEncoder = NULL;
	my_encoder.encoderFrm = MY_G711A_Encode_Frm;
	my_encoder.closeEncoder = NULL;
	ret = IMP_AENC_RegisterEncoder(&handle_g711a, &my_encoder);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_AENC_RegisterEncoder failed\n");
		return -1;
	}

	/* my G711U encoder Register */
	int handle_g711u = 0;
	memset(&my_encoder, 0x0, sizeof(my_encoder));
	my_encoder.maxFrmLen = 1024;
	sprintf(my_encoder.name, "%s", "MY_G711U");
	my_encoder.openEncoder = NULL;
	my_encoder.encoderFrm = MY_G711U_Encode_Frm;
	my_encoder.closeEncoder = NULL;
	ret = IMP_AENC_RegisterEncoder(&handle_g711u, &my_encoder);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_AENC_RegisterEncoder failed\n");
		return -1;
	}

	/* audio encode create channel. */
	int AeChn = 0;
	IMPAudioEncChnAttr attr;
	attr.type = handle_g711a; /* Use the My method to encoder. if use the system method is attr.type = PT_G711A; */
	attr.bufSize = 20;
	ret = IMP_AENC_CreateChn(AeChn, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "imp audio encode create channel failed\n");
		return -1;
	}

	while(1) {
		ret = fread(buf_pcm, 1, IMP_AUDIO_BUF_SIZE, file_pcm);
		if(ret < IMP_AUDIO_BUF_SIZE)
			break;

		/* Send a frame to encode. */
		IMPAudioFrame frm;
		frm.virAddr = (uint32_t *)buf_pcm;
		frm.len = ret;
		ret = IMP_AENC_SendFrame(AeChn, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
			return -1;
		}

		/* get audio encode frame. */
		IMPAudioStream stream;
		ret = IMP_AENC_PollingStream(AeChn, 1000);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "imp audio encode polling stream failed\n");
		}

		ret = IMP_AENC_GetStream(AeChn, &stream, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "imp audio encode get stream failed\n");
			return -1;
		}

		/* save the encode data to the file. */
		fwrite(stream.stream, 1, stream.len, file_g711);

		/* release stream. */
		ret = IMP_AENC_ReleaseStream(AeChn, &stream);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "imp audio encode release stream failed\n");
			return -1;
		}
	}

	ret = IMP_AENC_UnRegisterEncoder(&handle_g711a);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_AENC_UnRegisterEncoder failed\n");
		return -1;
	}

	ret = IMP_AENC_UnRegisterEncoder(&handle_g711u);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_AENC_UnRegisterEncoder failed\n");
		return -1;
	}

	/* destroy the encode channel. */
	ret = IMP_AENC_DestroyChn(AeChn);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "imp audio encode destroy channel failed\n");
		return -1;
	}

	fclose(file_pcm);
	fclose(file_g711);

	free(buf_pcm);
	return 0;
}

static int IMP_Audio_Decode(void)
{
	char *buf_g711 = NULL;
	int ret = -1;

	buf_g711 = (char *)malloc(IMP_AUDIO_BUF_SIZE);
	if(buf_g711 == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio g711 buf error\n", __func__);
		return -1;
	}

	FILE *file_pcm = fopen(IMP_AUDIO_PLAY_FILE, "wb");
	if(file_pcm == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, IMP_AUDIO_PLAY_FILE);
		return -1;
	}

	FILE *file_g711 = fopen(IMP_AUDIO_ENCODE_FILE, "rb");
	if(file_g711 == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, IMP_AUDIO_ENCODE_FILE);
		return -1;
	}

	/* My g711a decoder Register. */
	int handle_g711a = 0;
	IMPAudioDecDecoder my_decoder;
	sprintf(my_decoder.name, "%s", "MY_G711A");
	my_decoder.openDecoder = NULL;
	my_decoder.decodeFrm = MY_G711A_Decode_Frm;
	my_decoder.getFrmInfo = NULL;
	my_decoder.closeDecoder = NULL;

	ret = IMP_ADEC_RegisterDecoder(&handle_g711a, &my_decoder);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_ADEC_RegisterDecoder failed\n");
		return -1;
	}

	/* My g711u decoder Register. */
	int handle_g711u = 0;
	memset(&my_decoder, 0x0, sizeof(my_decoder));
	sprintf(my_decoder.name, "%s", "MY_G711U");
	my_decoder.openDecoder = NULL;
	my_decoder.decodeFrm = MY_G711U_Decode_Frm;
	my_decoder.getFrmInfo = NULL;
	my_decoder.closeDecoder = NULL;

	ret = IMP_ADEC_RegisterDecoder(&handle_g711u, &my_decoder);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_ADEC_RegisterDecoder failed\n");
		return -1;
	}

	/* audio decoder create channel. */
	int adChn = 0;
	IMPAudioDecChnAttr attr;
	attr.type = handle_g711a;
	attr.bufSize = 20;
	attr.mode = ADEC_MODE_PACK;
	ret = IMP_ADEC_CreateChn(adChn, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "imp audio decoder create channel failed\n");
		return -1;
	}

	ret = IMP_ADEC_ClearChnBuf(adChn);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_ADEC_ClearChnBuf failed\n");
		return -1;
	}

	while(1) {
		ret = fread(buf_g711, 1, IMP_AUDIO_BUF_SIZE/2, file_g711);
		if(ret < IMP_AUDIO_BUF_SIZE/2)
			break;

		/* Send a frame to decoder. */
		IMPAudioStream stream_in;
		stream_in.stream = (uint8_t *)buf_g711;
		stream_in.len = ret;
		ret = IMP_ADEC_SendStream(adChn, &stream_in, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
			return -1;
		}

		/* get audio decoder frame. */
		IMPAudioStream stream_out;
		ret = IMP_ADEC_PollingStream(adChn, 1000);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "imp audio encode polling stream failed\n");
		}

		ret = IMP_ADEC_GetStream(adChn, &stream_out, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "imp audio decoder get stream failed\n");
			return -1;
		}

		/* save the decoder data to the file. */
		fwrite(stream_out.stream, 1, stream_out.len, file_pcm);

		/* release stream. */
		ret = IMP_ADEC_ReleaseStream(adChn, &stream_out);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "imp audio decoder release stream failed\n");
			return -1;
		}
	}

	ret = IMP_ADEC_UnRegisterDecoder(&handle_g711a);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_ADEC_UnRegisterDecoder failed\n");
		return -1;
	}

	ret = IMP_ADEC_UnRegisterDecoder(&handle_g711u);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_ADEC_UnRegisterDecoder failed\n");
		return -1;
	}

	/* destroy the decoder channel. */
	ret = IMP_ADEC_DestroyChn(adChn);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "imp audio decoder destroy channel failed\n");
		return -1;
	}

	fclose(file_pcm);
	fclose(file_g711);

	free(buf_g711);
	return 0;
}

static void *IMP_Audio_Play_VolTest_Thread(void *argv)
{
	unsigned char *buf = NULL;
	int size = 0;
	int ret = -1;

	if(argv == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: Please input the play file name.\n", __func__);
		return NULL;
	}

	buf = (unsigned char *)malloc(IMP_AUDIO_BUF_SIZE);
	if(buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return NULL;
	}

	FILE *play_file = fopen(argv, "rb");
	if(play_file == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, IMP_AUDIO_RECORD_VOLTEST_FILE);
		return NULL;
	}

	/* Step 1: set public attribute of AO device. */
	int devID = 0;
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_8000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 400;
	attr.chnCnt = 1;
	ret = IMP_AO_SetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ao %d attr err: %d\n", devID, ret);
		return NULL;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AO_GetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ao %d attr err: %d\n", devID, ret);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr samplerate:%d\n", attr.samplerate);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr   bitwidth:%d\n", attr.bitwidth);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr  soundmode:%d\n", attr.soundmode);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr     frmNum:%d\n", attr.frmNum);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr  numPerFrm:%d\n", attr.numPerFrm);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr     chnCnt:%d\n", attr.chnCnt);

	/* Step 2: enable AO device. */
	ret = IMP_AO_Enable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "enable ao %d err\n", devID);
		return NULL;
	}

	/* Step 3: enable AI channel. */
	int chnID = 0;
	ret = IMP_AO_EnableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio play enable channel failed\n");
		return NULL;
	}

	/* Step 4: Set audio channel volume. */
	int chnVol = 90;
	ret = IMP_AO_SetVol(devID, chnID, chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play set volume failed\n");
		return NULL;
	}

	ret = IMP_AO_GetVol(devID, chnID, &chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play get volume failed\n");
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio Out GetVol    vol:%d\n", chnVol);

	int st = 0;
	while(1) {
		size = fread(buf, 1, IMP_AUDIO_BUF_SIZE, play_file);
		if(size < IMP_AUDIO_BUF_SIZE)
			break;

		/* Step 5: send frame data. */
		IMPAudioFrame frm;
		frm.virAddr = (uint32_t *)buf;
		frm.len = size;
		ret = IMP_AO_SendFrame(devID, chnID, &frm, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "send Frame Data error\n");
			return NULL;
		}

		st += size;
		if (st > 160256 ) {
			st = 0;
			chnVol -= 10;
			ret = IMP_AO_SetVol(devID, chnID, chnVol);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "Audio Play set volume failed\n");
				return NULL;
			}

			ret = IMP_AO_GetVol(devID, chnID, &chnVol);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "Audio Play get volume failed\n");
				return NULL;
			}

			IMP_LOG_INFO(TAG, "Audio Out GetVol    vol:%d\n", chnVol);
		}
	}

	/* Step 6: disable the audio channel. */
	ret = IMP_AO_DisableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio channel disable error\n");
		return NULL;
	}

	/* Step 7: disable the audio devices. */
	ret = IMP_AO_Disable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio device disable error\n");
		return NULL;
	}

	fclose(play_file);
	free(buf);
	pthread_exit(0);
}
static void *IMP_Audio_Record_VolTest_Thread(void *argv)
{
	int ret = -1;
	int record_num = 0;

	if(argv == NULL) {
		IMP_LOG_ERR(TAG, "Please input the record file name.\n");
		return NULL;
	}

	FILE *record_file = fopen(argv, "wb");
	if(record_file == NULL) {
		IMP_LOG_ERR(TAG, "fopen %s failed\n", argv);
		return NULL;
	}

	/* Step 1: set public attribute of AI device. */
	int devID = 1;
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_8000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 400;
	attr.chnCnt = 1;
	ret = IMP_AI_SetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ai %d attr err: %d\n", devID, ret);
		return NULL;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AI_GetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ai %d attr err: %d\n", devID, ret);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio In GetPubAttr samplerate : %d\n", attr.samplerate);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr   bitwidth : %d\n", attr.bitwidth);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  soundmode : %d\n", attr.soundmode);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr     frmNum : %d\n", attr.frmNum);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  numPerFrm : %d\n", attr.numPerFrm);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr     chnCnt : %d\n", attr.chnCnt);

	/* Step 2: enable AI device. */
	ret = IMP_AI_Enable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "enable ai %d err\n", devID);
		return NULL;
	}

	/* Step 3: set audio channel attribute of AI device. */
	int chnID = 0;
	IMPAudioIChnParam chnParam;
	chnParam.usrFrmDepth = 20;
	ret = IMP_AI_SetChnParam(devID, chnID, &chnParam);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ai %d channel %d attr err: %d\n", devID, chnID, ret);
		return NULL;
	}

	memset(&chnParam, 0x0, sizeof(chnParam));
	ret = IMP_AI_GetChnParam(devID, chnID, &chnParam);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ai %d channel %d attr err: %d\n", devID, chnID, ret);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio In GetChnParam usrFrmDepth : %d\n", chnParam.usrFrmDepth);

	/* Step 4: enable AI channel. */
	ret = IMP_AI_EnableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record enable channel failed\n");
		return NULL;
	}

	/* Step 5: Set audio channel volume. */
	int chnVol = 100;
	ret = IMP_AI_SetVol(devID, chnID, chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record set volume failed\n");
		return NULL;
	}

	ret = IMP_AI_GetVol(devID, chnID, &chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record get volume failed\n");
		return NULL;
	}
	IMP_LOG_INFO(TAG, "Audio In GetVol    vol : %d\n", chnVol);

	while(1) {
		/* Step 6: get audio record frame. */

		ret = IMP_AI_PollingFrame(devID, chnID, 1000);
		if (ret != 0 ) {
			IMP_LOG_ERR(TAG, "Audio Polling Frame Data error\n");
		}
		IMPAudioFrame frm;
		ret = IMP_AI_GetFrame(devID, chnID, &frm, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
			return NULL;
		}

		/* Step 7: Save the recording data to a file. */
		fwrite(frm.virAddr, 1, frm.len, record_file);

		/* Step 8: release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(devID, chnID, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio release frame data error\n");
			return NULL;
		}

		if(++record_num >= IMP_AUDIO_RECORD_NUM)
			break;
		if ((record_num % 200) == 0) {
			chnVol += 10;
			ret = IMP_AI_SetVol(devID, chnID, chnVol);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "Audio Record set volume failed\n");
				return NULL;
			}

			ret = IMP_AI_GetVol(devID, chnID, &chnVol);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "Audio Record get volume failed\n");
				return NULL;
			}
			IMP_LOG_INFO(TAG, "Audio In GetVol    vol : %d\n", chnVol);
		}
	}

	/* Step 9: disable the audio channel. */
	ret = IMP_AI_DisableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio channel disable error\n");
		return NULL;
	}

	/* Step 10: disable the audio devices. */
	ret = IMP_AI_Disable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio device disable error\n");
		return NULL;
	}

	fclose(record_file);
	pthread_exit(0);
}

static void *IMP_Audio_Record_Thread(void *argv)
{
	int ret = -1;
	int record_num = 0;

	if(argv == NULL) {
		IMP_LOG_ERR(TAG, "Please input the record file name.\n");
		return NULL;
	}

	FILE *record_file = fopen(argv, "wb");
	if(record_file == NULL) {
		IMP_LOG_ERR(TAG, "fopen %s failed\n", IMP_AUDIO_RECORD_FILE);
		return NULL;
	}

	/* Step 1: set public attribute of AI device. */
	int devID = 1;
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_8000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 400;
	attr.chnCnt = 1;
	ret = IMP_AI_SetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ai %d attr err: %d\n", devID, ret);
		return NULL;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AI_GetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ai %d attr err: %d\n", devID, ret);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio In GetPubAttr samplerate : %d\n", attr.samplerate);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr   bitwidth : %d\n", attr.bitwidth);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  soundmode : %d\n", attr.soundmode);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr     frmNum : %d\n", attr.frmNum);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  numPerFrm : %d\n", attr.numPerFrm);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr     chnCnt : %d\n", attr.chnCnt);

	/* Step 2: enable AI device. */
	ret = IMP_AI_Enable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "enable ai %d err\n", devID);
		return NULL;
	}

	/* Step 3: set audio channel attribute of AI device. */
	int chnID = 0;
	IMPAudioIChnParam chnParam;
	chnParam.usrFrmDepth = 20;
	ret = IMP_AI_SetChnParam(devID, chnID, &chnParam);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ai %d channel %d attr err: %d\n", devID, chnID, ret);
		return NULL;
	}

	memset(&chnParam, 0x0, sizeof(chnParam));
	ret = IMP_AI_GetChnParam(devID, chnID, &chnParam);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ai %d channel %d attr err: %d\n", devID, chnID, ret);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio In GetChnParam usrFrmDepth : %d\n", chnParam.usrFrmDepth);

	/* Step 4: enable AI channel. */
	ret = IMP_AI_EnableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record enable channel failed\n");
		return NULL;
	}

	/* Step 5: Set audio channel volume. */
	int chnVol = 60;
	ret = IMP_AI_SetVol(devID, chnID, chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record set volume failed\n");
		return NULL;
	}

	ret = IMP_AI_GetVol(devID, chnID, &chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record get volume failed\n");
		return NULL;
	}
	IMP_LOG_INFO(TAG, "Audio In GetVol    vol : %d\n", chnVol);

	int aigain = 20;
	ret = IMP_AI_SetGain(devID, chnID, aigain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Set Gain failed\n");
		return NULL;
	}

	ret = IMP_AI_GetGain(devID, chnID, &aigain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
		return NULL;
	}
	IMP_LOG_INFO(TAG, "Audio In GetGain    gain : %d\n", aigain);

	/* Enable Aec */
	ret = IMP_AI_EnableAec(devID, chnID, 0, 0);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio enable aec failed\n");
		return NULL;
	}

	while(1) {
		/* Step 6: get audio record frame. */

		ret = IMP_AI_PollingFrame(devID, chnID, 1000);
		if (ret != 0 ) {
			IMP_LOG_ERR(TAG, "Audio Polling Frame Data error\n");
		}
		IMPAudioFrame frm;
		ret = IMP_AI_GetFrame(devID, chnID, &frm, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
			return NULL;
		}

		/* Step 7: Save the recording data to a file. */
		fwrite(frm.virAddr, 1, frm.len, record_file);

		/* Step 8: release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(devID, chnID, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio release frame data error\n");
			return NULL;
		}

		if(++record_num >= IMP_AUDIO_RECORD_NUM)
			break;
	}

	/* Disable Aec */
	ret = IMP_AI_DisableAec(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio enable aec failed\n");
		return NULL;
	}

	/* Step 9: disable the audio channel. */
	ret = IMP_AI_DisableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio channel disable error\n");
		return NULL;
	}

	/* Step 10: disable the audio devices. */
	ret = IMP_AI_Disable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio device disable error\n");
		return NULL;
	}

	fclose(record_file);
	pthread_exit(0);
}

static void *IMP_Audio_Record_ALGO_AI_Thread(void *argv)
{
	int ret = -1;
	int record_num = 0;

	if(argv == NULL) {
		IMP_LOG_ERR(TAG, "Please input the record file name.\n");
		return NULL;
	}

	FILE *record_file = fopen(argv, "wb");
	if(record_file == NULL) {
		IMP_LOG_ERR(TAG, "fopen %s failed\n", IMP_AUDIO_RECORD_ALGO_AI_FILE);
		return NULL;
	}

	/* Step 1: set public attribute of AI device. */
	int devID = 1;
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_8000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 400;
	attr.chnCnt = 1;
	ret = IMP_AI_SetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ai %d attr err: %d\n", devID, ret);
		return NULL;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AI_GetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ai %d attr err: %d\n", devID, ret);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio In GetPubAttr samplerate : %d\n", attr.samplerate);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr   bitwidth : %d\n", attr.bitwidth);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  soundmode : %d\n", attr.soundmode);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr     frmNum : %d\n", attr.frmNum);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  numPerFrm : %d\n", attr.numPerFrm);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr     chnCnt : %d\n", attr.chnCnt);

	/* Step 2: enable AI device. */
	ret = IMP_AI_Enable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "enable ai %d err\n", devID);
		return NULL;
	}

	/* Step 3: set audio channel attribute of AI device. */
	int chnID = 0;
	IMPAudioIChnParam chnParam;
	chnParam.usrFrmDepth = 20;
	ret = IMP_AI_SetChnParam(devID, chnID, &chnParam);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ai %d channel %d attr err: %d\n", devID, chnID, ret);
		return NULL;
	}

	memset(&chnParam, 0x0, sizeof(chnParam));
	ret = IMP_AI_GetChnParam(devID, chnID, &chnParam);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ai %d channel %d attr err: %d\n", devID, chnID, ret);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio In GetChnParam usrFrmDepth : %d\n", chnParam.usrFrmDepth);

	/* Step 4: enable AI channel. */
	ret = IMP_AI_EnableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record enable channel failed\n");
		return NULL;
	}

	/* Step 5: Set audio channel volume. */
	int chnVol = 60;
	ret = IMP_AI_SetVol(devID, chnID, chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record set volume failed\n");
		return NULL;
	}

	ret = IMP_AI_GetVol(devID, chnID, &chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record get volume failed\n");
		return NULL;
	}
	IMP_LOG_INFO(TAG, "Audio In GetVol    vol : %d\n", chnVol);

	ret = IMP_AI_EnableHpf(&attr);
	if(ret != 0) {
		printf("enable audio hpf error.\n");
		IMP_LOG_INFO(TAG, "enable audio hpf error.\n");
		return NULL;
	}

	ret = IMP_AI_EnableNs(&attr, NS_VERYHIGH);
	if(ret != 0) {
		printf("enable audio ns error.\n");
		IMP_LOG_INFO(TAG, "enable audio ns error.\n");
		return NULL;
	}

	while(1) {
		/* Step 6: get audio record frame. */

		ret = IMP_AI_PollingFrame(devID, chnID, 1000);
		if (ret != 0 ) {
			IMP_LOG_ERR(TAG, "Audio Polling Frame Data error\n");
		}
		IMPAudioFrame frm;
		ret = IMP_AI_GetFrame(devID, chnID, &frm, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
			return NULL;
		}

		/* Step 7: Save the recording data to a file. */
		fwrite(frm.virAddr, 1, frm.len, record_file);

		/* Step 8: release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(devID, chnID, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio release frame data error\n");
			return NULL;
		}

		if(++record_num >= IMP_AUDIO_RECORD_NUM)
			break;

	}

	IMPAudioAgcConfig agcConfig = {
		.TargetLevelDbfs = 0,
		.CompressionGaindB = 6
	};

	ret = IMP_AI_EnableAgc(&attr, agcConfig);
	if(ret != 0) {
		printf("enable audio agc error.\n");
		IMP_LOG_INFO(TAG, "enable audio agc error.\n");
		return NULL;
	}

	record_num = 0;
	while(1) {
		/* Step 6: get audio record frame. */

		ret = IMP_AI_PollingFrame(devID, chnID, 1000);
		if (ret != 0 ) {
			IMP_LOG_ERR(TAG, "Audio Polling Frame Data error\n");
		}
		IMPAudioFrame frm;
		ret = IMP_AI_GetFrame(devID, chnID, &frm, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
			return NULL;
		}

		/* Step 7: Save the recording data to a file. */
		fwrite(frm.virAddr, 1, frm.len, record_file);

		/* Step 8: release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(devID, chnID, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio release frame data error\n");
			return NULL;
		}

		if(++record_num >= IMP_AUDIO_RECORD_NUM)
			break;

	}
	ret = IMP_AI_DisableHpf();
	if(ret != 0) {
		printf("enable audio hpf error.\n");
		IMP_LOG_INFO(TAG, "enable audio hpf error.\n");
		return NULL;
	}

	ret = IMP_AI_DisableNs();
	if(ret != 0) {
		printf("disable audio ns error.\n");
		IMP_LOG_INFO(TAG, "disable audio ns error.\n");
		return NULL;
	}

	ret = IMP_AI_DisableAgc();
	if(ret != 0) {
		printf("disable audio ns error.\n");
		IMP_LOG_INFO(TAG, "disable audio ns error.\n");
		return NULL;
	}

	record_num = 0;
	while(1) {
		/* Step 6: get audio record frame. */

		ret = IMP_AI_PollingFrame(devID, chnID, 1000);
		if (ret != 0 ) {
			IMP_LOG_ERR(TAG, "Audio Polling Frame Data error\n");
		}
		IMPAudioFrame frm;
		ret = IMP_AI_GetFrame(devID, chnID, &frm, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
			return NULL;
		}

		/* Step 7: Save the recording data to a file. */
		fwrite(frm.virAddr, 1, frm.len, record_file);

		/* Step 8: release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(devID, chnID, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio release frame data error\n");
			return NULL;
		}

		if(++record_num >= IMP_AUDIO_RECORD_NUM)
			break;

	}

	/* Step 9: disable the audio channel. */
	ret = IMP_AI_DisableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio channel disable error\n");
		return NULL;
	}

	/* Step 10: disable the audio devices. */
	ret = IMP_AI_Disable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio device disable error\n");
		return NULL;
	}

	fclose(record_file);
	pthread_exit(0);
}

static void *IMP_Audio_Play_Thread(void *argv)
{
	unsigned char *buf = NULL;
	int size = 0;
	int ret = -1;

	if(argv == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: Please input the play file name.\n", __func__);
		return NULL;
	}

	buf = (unsigned char *)malloc(IMP_AUDIO_BUF_SIZE);
	if(buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return NULL;
	}

	FILE *play_file = fopen(argv, "rb");
	if(play_file == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, IMP_AUDIO_PLAY_FILE);
		return NULL;
	}

	/* Step 1: set public attribute of AO device. */
	int devID = 0;
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_8000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 400;
	attr.chnCnt = 1;
	ret = IMP_AO_SetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ao %d attr err: %d\n", devID, ret);
		return NULL;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AO_GetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ao %d attr err: %d\n", devID, ret);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr samplerate:%d\n", attr.samplerate);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr   bitwidth:%d\n", attr.bitwidth);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr  soundmode:%d\n", attr.soundmode);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr     frmNum:%d\n", attr.frmNum);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr  numPerFrm:%d\n", attr.numPerFrm);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr     chnCnt:%d\n", attr.chnCnt);

	/* Step 2: enable AO device. */
	ret = IMP_AO_Enable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "enable ao %d err\n", devID);
		return NULL;
	}

	/* Step 3: enable AI channel. */
	int chnID = 0;
	ret = IMP_AO_EnableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio play enable channel failed\n");
		return NULL;
	}

	/* Step 4: Set audio channel volume. */
	int chnVol = 60;
	ret = IMP_AO_SetVol(devID, chnID, chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play set volume failed\n");
		return NULL;
	}

	ret = IMP_AO_GetVol(devID, chnID, &chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play get volume failed\n");
		return NULL;
	}
	IMP_LOG_INFO(TAG, "Audio Out GetVol    vol:%d\n", chnVol);

	int aogain = 28;
	ret = IMP_AO_SetGain(devID, chnID, aogain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Set Gain failed\n");
		return NULL;
	}

	ret = IMP_AO_GetGain(devID, chnID, &aogain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
		return NULL;
	}
	IMP_LOG_INFO(TAG, "Audio Out GetGain    gain : %d\n", aogain);

	int i = 0;
	while(1) {
		size = fread(buf, 1, IMP_AUDIO_BUF_SIZE, play_file);
		if(size < IMP_AUDIO_BUF_SIZE)
			break;

		/* Step 5: send frame data. */
		IMPAudioFrame frm;
		frm.virAddr = (uint32_t *)buf;
		frm.len = size;
		ret = IMP_AO_SendFrame(devID, chnID, &frm, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "send Frame Data error\n");
			return NULL;
		}

		IMPAudioOChnState play_status;
		ret = IMP_AO_QueryChnStat(devID, chnID, &play_status);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_AO_QueryChnStat error\n");
			return NULL;
		}

		IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
				play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);

		if(++i == 100) {
			ret = IMP_AO_PauseChn(devID, chnID);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_AO_PauseChn error\n");
				return NULL;
			}

			printf("[INFO] Test : Audio Play Pause test.\n");
			printf("[INFO]      : Please input any key to continue.\n");
			getchar();

			ret = IMP_AO_ClearChnBuf(devID, chnID);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_AO_ClearChnBuf error\n");
				return NULL;
			}

			ret = IMP_AO_ResumeChn(devID, chnID);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_AO_ResumeChn error\n");
				return NULL;
			}
		}
	}

	/* Step 6: disable the audio channel. */
	ret = IMP_AO_DisableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio channel disable error\n");
		return NULL;
	}

	/* Step 7: disable the audio devices. */
	ret = IMP_AO_Disable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio device disable error\n");
		return NULL;
	}

	fclose(play_file);
	free(buf);
	pthread_exit(0);
}

static void *IMP_Audio_Play_ALGO_AO_Thread(void *argv)
{
	unsigned char *buf = NULL;
	int size = 0;
	int ret = -1;

	if(argv == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: Please input the play file name.\n", __func__);
		return NULL;
	}

	buf = (unsigned char *)malloc(IMP_AUDIO_BUF_SIZE);
	if(buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return NULL;
	}

	FILE *play_file = fopen(argv, "rb");
	if(play_file == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, IMP_AUDIO_PLAY_FILE);
		return NULL;
	}

	/* Step 1: set public attribute of AO device. */
	int devID = 0;
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_8000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 400;
	attr.chnCnt = 1;
	ret = IMP_AO_SetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ao %d attr err: %d\n", devID, ret);
		return NULL;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AO_GetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ao %d attr err: %d\n", devID, ret);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr samplerate:%d\n", attr.samplerate);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr   bitwidth:%d\n", attr.bitwidth);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr  soundmode:%d\n", attr.soundmode);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr     frmNum:%d\n", attr.frmNum);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr  numPerFrm:%d\n", attr.numPerFrm);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr     chnCnt:%d\n", attr.chnCnt);

	/* Step 2: enable AO device. */
	ret = IMP_AO_Enable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "enable ao %d err\n", devID);
		return NULL;
	}

	/* Step 3: enable AI channel. */
	int chnID = 0;
	ret = IMP_AO_EnableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio play enable channel failed\n");
		return NULL;
	}

	/* Step 4: Set audio channel volume. */
	int chnVol = 60;
	ret = IMP_AO_SetVol(devID, chnID, chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play set volume failed\n");
		return NULL;
	}

	ret = IMP_AO_GetVol(devID, chnID, &chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play get volume failed\n");
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio Out GetVol    vol:%d\n", chnVol);

	ret = IMP_AO_EnableHpf(&attr);
	if(ret != 0) {
		printf("enable audio hpf error.\n");
		IMP_LOG_INFO(TAG, "enable audio hpf error.\n");
		return NULL;
	}

	int i = 0;
	while(1) {
		size = fread(buf, 1, IMP_AUDIO_BUF_SIZE, play_file);
		if(size < IMP_AUDIO_BUF_SIZE)
			break;

		/* Step 5: send frame data. */
		IMPAudioFrame frm;
		frm.virAddr = (uint32_t *)buf;
		frm.len = size;
		ret = IMP_AO_SendFrame(devID, chnID, &frm, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "send Frame Data error\n");
			return NULL;
		}

		IMPAudioOChnState play_status;
		ret = IMP_AO_QueryChnStat(devID, chnID, &play_status);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_AO_QueryChnStat error\n");
			return NULL;
		}

		IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
				play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);

		if(++i == 100) {
			ret = IMP_AO_PauseChn(devID, chnID);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_AO_PauseChn error\n");
				return NULL;
			}

			printf("[INFO] Test : Audio Play Pause test.\n");
			printf("[INFO]      : Please input any key to continue.\n");
			getchar();

			ret = IMP_AO_ClearChnBuf(devID, chnID);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_AO_ClearChnBuf error\n");
				return NULL;
			}

			ret = IMP_AO_ResumeChn(devID, chnID);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_AO_ResumeChn error\n");
				return NULL;
			}
		}
	}

	IMPAudioAgcConfig agcConfig = {
		.TargetLevelDbfs = 0,
		.CompressionGaindB = 9
	};

	ret = IMP_AO_EnableAgc(&attr, agcConfig);
	if(ret != 0) {
		printf("enable audio agc error.\n");
		IMP_LOG_INFO(TAG, "enable audio agc error.\n");
		return NULL;
	}

	i = 0;
	fseek(play_file, 0, 0);
	while(1) {
		size = fread(buf, 1, IMP_AUDIO_BUF_SIZE, play_file);
		if(size < IMP_AUDIO_BUF_SIZE)
			break;

		/* Step 5: send frame data. */
		IMPAudioFrame frm;
		frm.virAddr = (uint32_t *)buf;
		frm.len = size;
		ret = IMP_AO_SendFrame(devID, chnID, &frm, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "send Frame Data error\n");
			return NULL;
		}
	}

	ret = IMP_AO_DisableHpf();
	if(ret != 0) {
		printf("enable audio hpf error.\n");
		IMP_LOG_INFO(TAG, "enable audio hpf error.\n");
		return NULL;
	}

	ret = IMP_AO_DisableAgc();
	if(ret != 0) {
		printf("disable audio ns error.\n");
		IMP_LOG_INFO(TAG, "disable audio ns error.\n");
		return NULL;
	}

	/* Step 6: disable the audio channel. */
	ret = IMP_AO_DisableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio channel disable error\n");
		return NULL;
	}

	/* Step 7: disable the audio devices. */
	ret = IMP_AO_Disable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio device disable error\n");
		return NULL;
	}

	fclose(play_file);
	free(buf);
	pthread_exit(0);
}

static void *IMP_Audio_Play_MuteTest_Thread(void *argv)
{
	unsigned char *buf = NULL;
	int size = 0;
	int ret = -1;

	if(argv == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: Please input the play file name.\n", __func__);
		return NULL;
	}

	buf = (unsigned char *)malloc(IMP_AUDIO_BUF_SIZE);
	if(buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return NULL;
	}

	FILE *play_file = fopen(argv, "rb");
	if(play_file == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, IMP_AUDIO_RECORD_VOLTEST_FILE);
		return NULL;
	}

	/* Step 1: set public attribute of AO device. */
	int devID = 0;
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_8000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 400;
	attr.chnCnt = 1;
	ret = IMP_AO_SetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ao %d attr err: %d\n", devID, ret);
		return NULL;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AO_GetPubAttr(devID, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ao %d attr err: %d\n", devID, ret);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr samplerate:%d\n", attr.samplerate);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr   bitwidth:%d\n", attr.bitwidth);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr  soundmode:%d\n", attr.soundmode);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr     frmNum:%d\n", attr.frmNum);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr  numPerFrm:%d\n", attr.numPerFrm);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr     chnCnt:%d\n", attr.chnCnt);

	/* Step 2: enable AO device. */
	ret = IMP_AO_Enable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "enable ao %d err\n", devID);
		return NULL;
	}

	/* Step 3: enable AI channel. */
	int chnID = 0;
	ret = IMP_AO_EnableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio play enable channel failed\n");
		return NULL;
	}

	/* Step 4: Set audio channel volume. */
	int chnVol = 65;
	ret = IMP_AO_SetVol(devID, chnID, chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play set volume failed\n");
		return NULL;
	}

	ret = IMP_AO_GetVol(devID, chnID, &chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play get volume failed\n");
		return NULL;
	}

	printf("Audio Out GetVol    vol:%d\n", chnVol);
	IMP_LOG_INFO(TAG, "Audio Out GetVol    vol:%d\n", chnVol);

	int st = 0;
	static int v = 0;
	while(1) {
		size = fread(buf, 1, IMP_AUDIO_BUF_SIZE, play_file);
		if(size < IMP_AUDIO_BUF_SIZE)
			break;

		/* Step 5: send frame data. */
		IMPAudioFrame frm;
		frm.virAddr = (uint32_t *)buf;
		frm.len = size;
		ret = IMP_AO_SendFrame(devID, chnID, &frm, BLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "send Frame Data error\n");
			return NULL;
		}

		st += size;
out:
		if (st > 160256 ) {
			st = 0;
			if (v == 0) {
				ret = IMP_AO_SetVolMute(devID, chnID, 1);
				if(ret != 0) {
					IMP_LOG_ERR(TAG, "send Frame Data error\n");
					return NULL;
				}
				v++;
				goto out;
			}
			if (v == 1) {
				ret = IMP_AO_SetVolMute(devID, chnID, 0);
				if(ret != 0) {
					IMP_LOG_ERR(TAG, "send Frame Data error\n");
					return NULL;
				}
				v++;
				goto out;
			}
			if (v == 2) {
				ret = IMP_AO_Soft_Mute(devID, chnID);
				if(ret != 0) {
					IMP_LOG_ERR(TAG, "send Frame Data error\n");
					return NULL;
				}
				v++;
				goto out;
			}
			if (v == 3) {
				ret = IMP_AO_Soft_UNMute(devID, chnID);
				if(ret != 0) {
					IMP_LOG_ERR(TAG, "send Frame Data error\n");
					return NULL;
				}
				v++;
				goto out;
			}
		}
	}

	/* Step 6: disable the audio channel. */
	ret = IMP_AO_DisableChn(devID, chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio channel disable error\n");
		return NULL;
	}

	/* Step 7: disable the audio devices. */
	ret = IMP_AO_Disable(devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio device disable error\n");
		return NULL;
	}

	fclose(play_file);
	free(buf);
	pthread_exit(0);
}

int  main(int argc, char *argv[])
{
	int ret;

	pthread_t record_thread_id, play_thread_id;

	printf("[INFO] Test ai vol: Start audio record vol test.\n");
	printf("[INFO]        : Can create the %s file.\n", IMP_AUDIO_RECORD_VOLTEST_FILE);
	printf("[INFO]        : Please input any key to continue.\n");
	getchar();

	ret = pthread_create(&record_thread_id, NULL, IMP_Audio_Record_VolTest_Thread, IMP_AUDIO_RECORD_VOLTEST_FILE);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: pthread_create Audio Record failed\n", __func__);
		return -1;
	}
	pthread_join(record_thread_id, NULL);

	printf("[INFO] Test ao vol: Start audio play vol test.\n");
	printf("[INFO]       : Play the %s file.\n", IMP_AUDIO_RECORD_VOLTEST_FILE);
	printf("[INFO]       : Please input any key to continue.\n");
	getchar();
	ret = pthread_create(&play_thread_id, NULL, IMP_Audio_Play_VolTest_Thread, IMP_AUDIO_RECORD_VOLTEST_FILE);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: pthread_create Audio Play failed\n", __func__);
		return -1;
	}
	pthread_join(play_thread_id, NULL);
#ifdef toto
	printf("[INFO] Test algo ai: Start audio record algo ai test.\n");
	printf("[INFO]        : Can create the %s file.\n", IMP_AUDIO_RECORD_ALGO_AI_FILE);
	printf("[INFO]        : Please input any key to continue.\n");
	getchar();

	ret = pthread_create(&record_thread_id, NULL, IMP_Audio_Record_ALGO_AI_Thread, IMP_AUDIO_RECORD_ALGO_AI_FILE);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: pthread_create Audio Record failed\n", __func__);
		return -1;
	}
	pthread_join(record_thread_id, NULL);

	printf("[INFO] Test algo ao: Start audio play algo ao test.\n");
	printf("[INFO]       : Play the %s file.\n", IMP_AUDIO_RECORD_ALGO_AI_FILE);
	printf("[INFO]       : Please input any key to continue.\n");
	getchar();

	ret = pthread_create(&play_thread_id, NULL, IMP_Audio_Play_ALGO_AO_Thread, IMP_AUDIO_RECORD_ALGO_AI_FILE);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: pthread_create Audio Play failed\n", __func__);
		return -1;
	}
	pthread_join(play_thread_id, NULL);

	printf("[INFO] Test 1: Start audio record test.\n");
	printf("[INFO]        : Can create the %s file.\n", IMP_AUDIO_RECORD_FILE);
	printf("[INFO]        : Please input any key to continue.\n");
	getchar();

	/* Step 1: Start audio recording thread. */
	ret = pthread_create(&record_thread_id, NULL, IMP_Audio_Record_Thread, IMP_AUDIO_RECORD_FILE);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: pthread_create Audio Record failed\n", __func__);
		return -1;
	}
	pthread_join(record_thread_id, NULL);

	printf("[INFO] Test 2: Start audio data encode.\n");
	printf("[INFO]       : PCM ---> G711, %s ---> %s\n", IMP_AUDIO_RECORD_FILE, IMP_AUDIO_ENCODE_FILE);
	printf("[INFO]       : Can create the %s file.\n", IMP_AUDIO_ENCODE_FILE);
	printf("[INFO]       : Please input any key to continue.\n");
	getchar();

	/* Step 2: Start audio encode. */
	ret = IMP_Audio_Encode();
	if(ret < 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: IMP_Audio_Encode failed\n", __func__);
		return -1;
	}

	printf("[INFO] Test 3: Start audio data decode.\n");
	printf("[INFO]       : G711 ---> PCM, %s ---> %s\n", IMP_AUDIO_ENCODE_FILE, IMP_AUDIO_PLAY_FILE);
	printf("[INFO]       : Can create the %s file.\n", IMP_AUDIO_PLAY_FILE);
	printf("[INFO]       : Please input any key to continue.\n");
	getchar();

	/* Step 3: Start audio decode. */
	ret = IMP_Audio_Decode();
	if(ret < 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: IMP_Audio_Decode failed\n", __func__);
		return -1;
	}

	printf("[INFO] Test 4: Start audio play test.\n");
	printf("[INFO]       : Play the %s file.\n", IMP_AUDIO_PLAY_FILE);
	printf("[INFO]       : Please input any key to continue.\n");
	getchar();

	/* Step 4: Start audio play test. */
	ret = pthread_create(&play_thread_id, NULL, IMP_Audio_Play_Thread, IMP_AUDIO_PLAY_FILE);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: pthread_create Audio Play failed\n", __func__);
		return -1;
	}
	pthread_join(play_thread_id, NULL);

	printf("[INFO] Test 5: Start AEC test.\n");
	printf("[INFO]       : Play the %s file.\n", IMP_AUDIO_PLAY_FILE);
	printf("[INFO]       : Can create the %s file.\n", IMP_AUDIO_FILTER_FILE);
	printf("[INFO]       : Please input any key to continue.\n");
	getchar();

	/* Step 5: Start AEC test. */
	ret = pthread_create(&play_thread_id, NULL, IMP_Audio_Play_Thread, IMP_AUDIO_PLAY_FILE);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: pthread_create Audio Play failed\n", __func__);
		return -1;
	}

	ret = pthread_create(&record_thread_id, NULL, IMP_Audio_Record_Thread, IMP_AUDIO_FILTER_FILE);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: pthread_create Audio Record failed\n", __func__);
		return -1;
	}

	pthread_join(play_thread_id, NULL);
	pthread_join(record_thread_id, NULL);

	printf("[INFO] Test 6: Start audio play test.\n");
	printf("[INFO]       : Play the %s file.\n", IMP_AUDIO_FILTER_FILE);
	printf("[INFO]       : Please input any key to continue.\n");
	getchar();

	/* Step 6: Start audio play test. */
	ret = pthread_create(&play_thread_id, NULL, IMP_Audio_Play_Thread, IMP_AUDIO_FILTER_FILE);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: pthread_create Audio Play failed\n", __func__);
		return -1;
	}
	pthread_join(play_thread_id, NULL);

	printf("[INFO] Test ao vol: Start audio play vol test.\n");
	printf("[INFO]       : Play the %s file.\n", IMP_AUDIO_RECORD_VOLTEST_FILE);
	printf("[INFO]       : Please input any key to continue.\n");
	getchar();
	ret = pthread_create(&play_thread_id, NULL, IMP_Audio_Play_MuteTest_Thread, IMP_AUDIO_RECORD_VOLTEST_FILE);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: pthread_create Audio Play failed\n", __func__);
		return -1;
	}
	pthread_join(play_thread_id, NULL);
#endif
	return 0;
}
