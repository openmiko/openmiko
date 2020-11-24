import ure as re
import picoweb
import signal
import ulogging
import uasyncio as asyncio
import uerrno
import machine
import time
import utemplate
import os
import gc

ulogging.basicConfig(level=ulogging.INFO)
# ulogging.basicConfig(level=logging.DEBUG)

log = ulogging.getLogger(__name__)


def handle_signal(signalNumber):
    print("Received:", signalNumber)
    return


class OpenMikoWebApp(picoweb.WebApp):
    def init(self):
        """Initialize the OpenMiko web application. """
        log.info("Initializing OpenMiko application")
        self.setup_signal_handlers()
        self.inited = True

    def setup_signal_handlers(self):
        log.info("Setting up signal handlers")
        SIGUSR1 = 10
        signal.signal(SIGUSR1, handle_signal)


def get_image(req, resp):
    pass


def update_settings(req, resp):
    pass


def get_webrtc(req, resp):
    pass


def ir_led(req, resp):
    """
    Control the infrared led.
    """

    if req.method in ["POST", "PUT"]:
        yield from req.read_form_data()

        pin49 = machine.Pin(49, machine.Pin.OUT)

        # For some reason the sleeps are necessary here
        if req.form["value"] == "1":
            pin49.value(1)
        else:
            pin49.value(0)

    yield from picoweb.start_response(resp)


def ir_cut(req, resp):
    """
    Control the infrared cut-off filter used to block mid-infrared
    wavelengths while passing visible light. Normally you want this on.
    Turning it off results in a purplish hue.
    """

    if req.method in ["POST", "PUT"]:
        yield from req.read_form_data()
        print(req.form)

        pin25 = machine.Pin(25, machine.Pin.OUT)
        pin26 = machine.Pin(26, machine.Pin.OUT)

        # For some reason the sleeps are necessary here
        if req.form["value"] == "1":
            pin25.value(0)
            pin26.value(1)
            time.sleep(1)
            pin26.value(0)
        else:
            pin26.value(0)
            pin25.value(1)
            time.sleep(1)
            pin25.value(0)

    yield from picoweb.start_response(resp)


def index(req, resp):
    # Or can use a convenience function start_response() (see its source for
    # extra params it takes).
    headers = {"Access-Control-Allow-Origin": "*"}

    yield from picoweb.start_response(resp, headers=headers)
    yield from app.render_template(resp, "index.html", (req,))


def cameraimage(req, resp):
    yield from picoweb.start_response(
        resp, content_type="multipart/x-mixed-replace;boundary=--boundarydonotcross"
    )

    chunk_size = 1024

    while True:
        os.system("/usr/bin/getimage")
        file_length = os.stat("/tmp/out.jpg")[6]
        f = open("/tmp/out.jpg", "rb")

        yield from resp.awrite("--boundarydonotcross\r\n")
        yield from resp.awrite("Content-type: image/jpeg\r\n")
        yield from resp.awrite("Content-length: %s\r\n\r\n" % file_length)

        filedata = f.read(chunk_size)
        while filedata:
            yield from resp.awrite(filedata)
            filedata = f.read(chunk_size)
        yield from resp.awrite("\r\n")

        yield from resp.awrite("--boundarydonotcross\r\n")
        time.sleep(1)


# def hello(req, resp):
#     yield from picoweb.start_response(resp)
#     # Here's how you extract matched groups from a regex URI match
#     yield from resp.awrite("Hello " + req.url_match.group(1))


ROUTES = [
    # You can specify exact URI string matches...
    ("/", index),
    ("/api/cameraimage", cameraimage),
    ("/api/ir_cut", ir_cut),
    ("/api/ir_led", ir_led),
]


app = OpenMikoWebApp(__name__, ROUTES)
# debug values:
# -1 disable all logging
# 0 (False) normal logging: requests and errors
# 1 (True) debug logging
# 2 extra debug logging
app.run(debug=1, host="0.0.0.0")
