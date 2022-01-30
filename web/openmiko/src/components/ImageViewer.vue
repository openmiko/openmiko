<template>
  <div>

    <div class="actions">
      <div class="btn-group" role="group" aria-label="Basic example">
        <button type="button" class="btn btn-secondary">Download</button>
      </div>
    </div>

    <div class="image-container">
      <img v-bind:src="imgsrc" />
    </div>
    
  </div>
</template>

<script>
import axios from 'axios';

export default {
  name: 'ImageViewer',
  data () {
    return {
      // Blank placeholder image
      imgsrc: "data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7"
    }
  },
  mounted () {

    var CAMERA_BASEURL = process.env.VUE_APP_CAMERA_BASEURL;
    var url = `${CAMERA_BASEURL}/camera/snap`

    axios
      .get(url, { responseType: 'arraybuffer' })
      .then(response => {

        const base64 = btoa(
          new Uint8Array(response.data).reduce(
            (data, byte) => data + String.fromCharCode(byte),
            '',
          ),
        );
        this.imgsrc = "data:image/jpeg;base64," + base64
      })
  },

}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
img {
  max-width: 100%;
}

.actions {
  margin-top: 12px;
  margin-bottom: 4px;
}

.image-container {
  width: 1000px;
  margin-left: auto;
  margin-right: auto;
}


</style>
