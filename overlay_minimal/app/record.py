import json


configfile = open("/etc/videocapture_settings.json", "r")
config = json.loads(configfile.read())

print(config)
