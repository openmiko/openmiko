
Vue.component('image-viewer', {
  data: function () {
    return {
      count: 0
    }
  },
  template: '<button class="btn btn-primary" v-on:click="count++">You clicked me {{ count }} times.</button>'
})

new Vue({ el: '#app' })