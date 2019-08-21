from os.path import join, isdir

Import("env")

FRAMEWORK_DIR = env.PioPlatform().get_package_dir("framework-arduinosam")

# skip patch process if we did it before
if not isdir(join(FRAMEWORK_DIR, "variants", "mendeleev")):
    variants_folder = join(FRAMEWORK_DIR, "variants")
    src_folder = join("variant")

    assert isdir(variants_folder) and isdir(src_folder)

    env.Execute("cp -r %s %s/mendeleev" % (src_folder, variants_folder))
