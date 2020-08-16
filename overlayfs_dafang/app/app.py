import os
import sys
import io

from flask import Flask
from flask_restful import Resource, Api
from flask import send_file

app = Flask(__name__, static_folder="static")
api = Api(app)


class HelloWorld(Resource):
    def get(self):
        return {"hello": "world"}


api.add_resource(HelloWorld, "/")


class CameraImageResource(Resource):
    def get(self):
        return {}


api.add_resource(CameraImageResource, "/cameraimages")


class CurrentImageResource(Resource):
    def get(self):

        stream = os.popen("/usr/bin/getimage")
        output = stream.read()

        return send_file(io.BytesIO(output), mimetype="image/jpeg")


api.add_resource(CurrentImageResource, "/camera/snap")

if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0")
