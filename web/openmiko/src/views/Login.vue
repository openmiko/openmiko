<template>
  <div class="home">
    <div class="container">
      <div class="row">
        <div class="col-sm-6 mx-auto">
          <form>
            <h2>Login</h2>

            <div class="form-outline mb-4">
              <label class="form-label" for="form2Example11">Email</label>
              <input
                type="text"
                id="form2Example11"
                class="form-control"
                placeholder="Email"
                v-model="form.email"
              />
            </div>

            <div class="form-outline mb-4">
              <label class="form-label" for="form2Example22">Password</label>
              <input
                type="password"
                id="form2Example22"
                class="form-control"
                v-model="form.password"
              />
            </div>

            <div class="text-center pt-1 mb-5 pb-1">
              <button
                class="btn btn-primary btn-block fa-lg gradient-custom-2 mb-3"
                type="button"
                v-on:click="login"
              >
                Log in
              </button>
            </div>
          </form>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
// const API_URL = "https://192.168.0.154/api/auth";

export default {
  name: "Login",
  components: {},
  data: function () {
    return {
      form: {
        email: "",
        password: "",
      },
    };
  },
  methods: {
    login: function () {
      this.$store
        .dispatch("login", {
          email: this.form.email,
          password: this.form.password,
        })
        .then(() => {
          this.$router.push("/");
        })
        .catch((err) => {
          console.log(err);
        });
      // try {
      //   axios.post(API_URL, this.form).then((res) => {
      //     if (res.data.token) {
      //       // Set cookie
      //       this.$store.dispatch("login", { token: res.token });
      //     }
      //   });
      //   this.success = true;
      // } catch (err) {
      //   this.error = err.message;
      // }
    },
  },
};
</script>
