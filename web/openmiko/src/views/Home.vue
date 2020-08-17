<template>
  <div class="home">
    <h1>This is a Home page</h1>
    <ImageViewer></ImageViewer>
    {{ msg }}
  </div>
</template>

<script>
import ImageViewer from '@/components/ImageViewer.vue';

export default {
  name: 'Home',
  components: {
    ImageViewer
  },
  data () {
    return {
      msg: 'Welcome to Your Vue.js App'
    }
  }
}
</script>
