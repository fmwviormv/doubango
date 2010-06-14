/**@file tdav_consumer_coreaudio.c
 * @brief Audio Consumer for MacOSX and iOS platforms.
 *
 * @author Laurent Etiemble <laurent.etiemble(at)gmail.com>
 *
 * @date Created: Sat Nov 8 16:54:58 2009 letiemble
 */
#include "tinydav/audio/coreaudio/tdav_consumer_coreaudio.h"

#include "tsk_thread.h"
#include "tsk_memory.h"
#include "tsk_debug.h"

#if HAVE_COREAUDIO

static void __handle_output_buffer(void *userdata, AudioQueueRef queue, AudioQueueBufferRef buffer) {
    OSStatus ret;
	void *data;
    tdav_consumer_coreaudio_t* consumer = (tdav_consumer_coreaudio_t*)userdata;

    if (!consumer->started) {
        return;
    }
    
	if((data = tdav_consumer_audio_get(TDAV_CONSUMER_AUDIO(consumer)))){
		memcpy(buffer->mAudioData, data, consumer->buffer_size);
		TSK_FREE(data);
	} else{
        memset(buffer->mAudioData, 0, consumer->buffer_size);
	}
    
    ret = AudioQueueEnqueueBuffer(consumer->queue, buffer, 0, NULL);
}

/* ============ Media Consumer Interface ================= */
int tdav_consumer_coreaudio_prepare(tmedia_consumer_t* self, const tmedia_codec_t* codec)
{
    OSStatus ret;
	tsk_size_t i;
	tdav_consumer_coreaudio_t* consumer = (tdav_consumer_coreaudio_t*)self;

	if(!consumer || !codec && codec->plugin){
		TSK_DEBUG_ERROR("Invalid parameter");
		return -1;
	}
	
	TDAV_CONSUMER_AUDIO(consumer)->channels = codec->plugin->audio.channels;
	TDAV_CONSUMER_AUDIO(consumer)->rate = codec->plugin->rate;
	/* codec should have ptime */

    // Create the audio stream description
    AudioStreamBasicDescription *description = &(consumer->description);
    description->mSampleRate = TDAV_CONSUMER_AUDIO(consumer)->rate;
    description->mFormatID = kAudioFormatLinearPCM;
    description->mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    description->mChannelsPerFrame = TDAV_CONSUMER_AUDIO(consumer)->channels;
    description->mFramesPerPacket = 1;
    description->mBitsPerChannel = TDAV_CONSUMER_AUDIO(consumer)->bits_per_sample;
    description->mBytesPerPacket = description->mBitsPerChannel / 8 * description->mChannelsPerFrame;
    description->mBytesPerFrame = description->mBytesPerPacket;
    description->mReserved = 0;
    
    int packetperbuffer = 1000 / TDAV_CONSUMER_AUDIO(consumer)->ptime;
    consumer->buffer_size = description->mSampleRate * description->mBytesPerFrame / packetperbuffer;
    
    // Create the playback audio queue
    ret = AudioQueueNewOutput(&(consumer->description),
                              __handle_output_buffer,
                              consumer,
                              NULL, 
                              NULL,
                              0,
                              &(consumer->queue));
    
    for(i = 0; i < kNumberBuffers; i++) {
        // Create the buffer for the queue
        ret = AudioQueueAllocateBuffer(consumer->queue, consumer->buffer_size, &(consumer->buffers[i]));
        if (ret) {
            break;
        }
        
        // Clear the data
        memset(consumer->buffers[i]->mAudioData, 0, consumer->buffer_size);
        consumer->buffers[i]->mAudioDataByteSize = consumer->buffer_size;
        
        // Enqueue the buffer
        ret = AudioQueueEnqueueBuffer(consumer->queue, consumer->buffers[i], 0, NULL);
        if (ret) {
            break;
        }
    }
    
	return ret;
}

int tdav_consumer_coreaudio_start(tmedia_consumer_t* self)
{
    OSStatus ret;
	tdav_consumer_coreaudio_t* consumer = (tdav_consumer_coreaudio_t*)self;

	if(!consumer){
		TSK_DEBUG_ERROR("Invalid parameter");
		return -1;
	}

	if(consumer->started){
		TSK_DEBUG_WARN("Consumer already started");
		return 0;
	}

	consumer->started = tsk_true;
    ret = AudioQueueStart(consumer->queue, NULL);
    
	return ret;
}

int tdav_consumer_coreaudio_consume(tmedia_consumer_t* self, void** buffer, tsk_size_t size)
{
	tdav_consumer_coreaudio_t* consumer = (tdav_consumer_coreaudio_t*)self;

	if(!consumer || !buffer || !*buffer || !size){
		TSK_DEBUG_ERROR("Invalid parameter");
		return -1;
	}
	/* buffer is already decoded */
	return tdav_consumer_audio_put(TDAV_CONSUMER_AUDIO(consumer), buffer);
}

int tdav_consumer_coreaudio_pause(tmedia_consumer_t* self)
{
    OSStatus ret;
	tdav_consumer_coreaudio_t* consumer = (tdav_consumer_coreaudio_t*)self;

	if(!consumer){
		TSK_DEBUG_ERROR("Invalid parameter");
		return -1;
	}

    ret = AudioQueuePause(consumer->queue);

	return ret;
}

int tdav_consumer_coreaudio_stop(tmedia_consumer_t* self)
{
    OSStatus ret;
	tdav_consumer_coreaudio_t* consumer = (tdav_consumer_coreaudio_t*)self;

	if(!self){
		TSK_DEBUG_ERROR("Invalid parameter");
		return -1;
	}

	if(!consumer->started){
		TSK_DEBUG_WARN("Consumer not started");
		return 0;
	}

	consumer->started = tsk_false;
    ret = AudioQueueStop(consumer->queue, false);
    
	return ret;
}

//
//	coreaudio consumer object definition
//
/* constructor */
static tsk_object_t* tdav_consumer_coreaudio_ctor(tsk_object_t * self, va_list * app)
{
	tdav_consumer_coreaudio_t *consumer = self;
	if(consumer){
		/* init base */
		tdav_consumer_audio_init(TDAV_CONSUMER_AUDIO(consumer));
	}
	return self;
}
/* destructor */
static tsk_object_t* tdav_consumer_coreaudio_dtor(tsk_object_t * self)
{ 
	tdav_consumer_coreaudio_t *consumer = self;
	if(consumer){
        if (consumer->queue) {
            AudioQueueDispose(consumer->queue, true);
        }
        
		/* deinit base */
		tdav_consumer_audio_deinit(TDAV_CONSUMER_AUDIO(consumer));
	}

	return self;
}

/* object definition */
static const tsk_object_def_t tdav_consumer_coreaudio_def_s = 
{
	sizeof(tdav_consumer_coreaudio_t),
	tdav_consumer_coreaudio_ctor, 
	tdav_consumer_coreaudio_dtor,
	tdav_consumer_audio_cmp, 
};

/* plugin definition*/
static const tmedia_consumer_plugin_def_t tmedia_consumer_coreaudio_plugin_def_s = 
{
	&tdav_consumer_coreaudio_def_s,
	
	tmedia_audio,
	"Apple CoreAudio consumer",
	
	tdav_consumer_coreaudio_prepare,
	tdav_consumer_coreaudio_start,
	tdav_consumer_coreaudio_consume,
	tdav_consumer_coreaudio_pause,
	tdav_consumer_coreaudio_stop
};

const tmedia_consumer_plugin_def_t *tmedia_consumer_coreaudio_plugin_def_t = &tmedia_consumer_coreaudio_plugin_def_s;

#endif /* HAVE_COREAUDIO */
