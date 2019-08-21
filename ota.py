try:
    import configparser
except ImportError:
    import ConfigParser as configparser
Import("env")

config = configparser.ConfigParser()
config.read("platformio.ini")
kast = config.get("common", "target_kast")

# Python callback
def on_upload(source, target, env):
    print("on_upload")
    fota(str(source[0]), kast, env)

def on_ota(*args, **kwargs):
    print("on_ota")
    fota("$BUILD_DIR/firmware.bin", kast, env)

def fota(file, kast, env):
    print("Uploading %s to element %s..." % (file, kast))
    env.Execute("cat %s | mosquitto_pub -h 192.168.1.2 -t mendeleev/%s/ota -s" % (file, kast))

env.Replace(UPLOADCMD=on_upload)
env.AlwaysBuild(env.Alias("ota",
    "$BUILD_DIR/firmware.bin",
    on_ota))
