const fs = require('fs');

module.exports = {
    productionSourceMap: false,
    devServer: {
      host: 'r1.zspin.com',
      https: {
        key: fs.readFileSync('../../certs/key.pem'),
        cert: fs.readFileSync('../../certs/cert.pem'),
      },
      disableHostCheck: true,
      port: 5003
    }
}
