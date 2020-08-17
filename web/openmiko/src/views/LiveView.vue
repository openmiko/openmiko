<template>
  <div class="liveview">
    <h1>This is a live webrtc view</h1>
    {{ msg }}
  </div>
</template>



<script>

export default {
  name: 'LiveView',
  data () {
    return {
      msg: process.env.VUE_APP_CAMERA_BASEURL
    }
  }
}
</script>
