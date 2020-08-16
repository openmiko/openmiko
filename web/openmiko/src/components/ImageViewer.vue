<template>
  <div>

    <div class="actions">
      <div class="btn-group" role="group" aria-label="Basic example">
        <button type="button" class="btn btn-secondary">Download</button>
      </div>
    </div>

    <div class="image-container">
      <img src="https://picsum.photos/1920/1080/"/>
    </div>
    
  </div>
</template>

<script>
export default {
  name: 'ImageViewer',
  props: {
    msg: String
  }
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
