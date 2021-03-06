import json
import pathlib

from yaml.events import NodeEvent
import watcher
import hashlib
import cleaner
import os
import yaml


from builders import azure_iot_config
from builders import class_bindings


# declare dictionaries
signatures_block = {}
timer_block = {}
variables_block = {}
handlers_block = {}
includes_block = {}
templates = {}
binding_variables = {}
manifest_updates = {}

device_twins_updates = None
device_twin_variables = None
dt = None
builders = None


code_lines = []

# generated_project_path = '../GenX_Generated'
generated_project_path = None


def load_bindings():
    global signatures_block, timer_block, variables_block, handlers_block, templates, builders, binding_variables, dt, includes_block, manifest_updates, generated_project_path

    signatures_block = {}
    timer_block = {}
    variables_block = {}
    handlers_block = {}
    includes_block = {}
    templates = {}
    binding_variables = {}
    manifest_updates = {}

    with open(r'app_model.yaml', 'r') as file:
        data = yaml.load(file, Loader=yaml.FullLoader)
        generated_project_path = data["genx"]["project_path"]

    classes = class_bindings.Builder(data, templates=templates, signatures_block=signatures_block, timer_block=timer_block, variables_block=variables_block,
                                     handlers_block=handlers_block, includes_block=includes_block)

    azure_iot = azure_iot_config.Builder(
        data, manifest_updates=manifest_updates)

    with open(generated_project_path + "/app_manifest.json", "r") as f:
        manifest = json.load(f)

    manifest_updates = azure_iot.build(manifest)
    manifest_updates = classes.build(manifest_updates)


def get_value(properties, key, default):
    if properties is None or key is None:
        return default
    return properties.get(key, default)


def write_comment_block(f, msg):
    f.write("\n/****************************************************************************************\n")
    f.write("* {msg}\n".format(msg=msg))
    f.write("****************************************************************************************/\n")


def load_templates():
    for path in pathlib.Path("templates").iterdir():
        if path.is_file():
            template_key = path.name.split(".")[0]
            with open(path, "r") as tf:
                templates.update({template_key: tf.read()})


def render_signatures(f):
    for item in sorted(signatures_block):
        sig = signatures_block.get(item)
        name = sig.get('name')
        template_key = sig.get('signature_template')
        f.write(templates[template_key].format(name=name))


def get_context_type(context_name):
    context_type = "void"
    # Search declared variables for matching variable name to get the type
    if context_name is not None:
        for variable_item in variables_block:
            variable = variables_block.get(variable_item)
            if context_name == variable.get('name'):
                context_type = variable['variable_template']['binding']
                break

    return context_type


def generate_timespec(properties, property_name):
    if properties is None:
        return "NULL"
    timespec = properties.get(property_name)
    if timespec is None:
        return "NULL"
    else:
        seconds = timespec.get('seconds')
        nanoseconds = timespec.get('nanoseconds')
        if seconds is not None and nanoseconds is not None:
            if type(seconds) is int:
                seconds = str(seconds)
            if type(nanoseconds) is int:
                nanoseconds = str(nanoseconds)

            timespec = "&(struct timespec){ " + \
                seconds + ", " + nanoseconds + " }"
            properties[property_name] = timespec
            return timespec


def render_variable(f, bindings_tags, var):
    device_twin_types = {"int": "DX_DEVICE_TWIN_INT", "float": "DX_DEVICE_TWIN_FLOAT", "double": "DX_DEVICE_TWIN_DOUBLE",
                         "bool": "DX_DEVICE_TWIN_BOOL",  "string": "DX_DEVICE_TWIN_STRING"}

    delay = None
    repeat = None

    name = var.get('name')

    properties = var.get('properties')
    variable_template = var.get('variable_template')

    template_key = variable_template.get('declare')
    binding = variable_template.get('binding')

    var_list = binding_variables.get(binding, [])
    var_list.append(name)
    binding_variables.update({binding: var_list})

    pin = get_value(properties, 'pin', 'GX_PIN_NOT_DECLARED_IN_GENX_MODEL')
    initialState = get_value(properties, 'initialState', 'GPIO_Value_Low')
    invert = "true" if get_value(
        properties, 'invertPin', True) else "false"

    type = get_value(properties, 'type', None)
    # twin_type = get_value(properties, 'type', None)
    twin_type = device_twin_types.get(type, 'DX_TYPE_UNKNOWN')

    detect = get_value(properties, 'detect', 'DX_GPIO_DETECT_LOW')

    repeat = generate_timespec(properties, 'repeat')
    delay = generate_timespec(properties, 'delay')

    context = get_value(properties, 'context', None)

    context_name = ""
    if context is not None:
        context_name = context.get('name', '')
        context_type = context.get('type', None)

        if context_type is None:
            context_type = get_context_type(context_name)

        if context_type is not None:
            context_name_prefix = bindings_tags.get(context_type, None)
            if context_name_prefix is not None:
                context_name = context_name_prefix + "_" + context_name

    if template_key is None or templates.get(template_key) is None:
        print('Key: "{template_key}" not found'.format(
            template_key=template_key))
        return
    else:
        f.write(templates[template_key].format(
            name=name,
            pin=pin,
            initialState=initialState,
            invert=invert,
            detect=detect,
            twin_type=twin_type,
            repeat=repeat,
            delay=delay,
            type=type,
            context_name=context_name
        ))


def render_variable_block(f):
    bindings_tags = yaml.load(templates.get(
        'declare_bindings_tag'), Loader=yaml.FullLoader)

    for tag in bindings_tags:
        for item in variables_block:
            var = variables_block.get(item)
            variable_template = var.get('variable_template')
            binding = variable_template.get('binding')
            if binding is not None and binding == tag:
                render_variable(f, bindings_tags, var)


def does_handler_exist(code_lines, handler):
    sig = handler + '_gx_handler'
    for line in code_lines:
        if sig in line:
            return True

    return False


def render_handler_block():
    for item in handlers_block:
        var = handlers_block.get(item)
        binding = var.get('binding')
        type = None
        lambda_property = ""
        context_type = ""
        context_name = ""

        # if binding is not None and binding == key_binding:

        name = var.get('name')
        properties = var.get('properties')
        if properties is not None:
            type = properties.get('type')
            lambda_property = properties.get('lambda', '')
            context = properties.get('context', None)
            if context is not None:
                context_type = context.get('type')
                context_name = context.get('name')

                # Search declared variables for matching variable name to get the type
                if context_type is None and context_name is not None:
                    context_type = get_context_type(context_name)

        if does_handler_exist(code_lines=code_lines, handler=name):
            continue

        template_key = var.get('handler_template')
        if template_key is None or templates.get(template_key) is None:
            print('Key: "{template_key}" not found'.format(
                template_key=template_key))
            continue
        else:
            try:
                block_chars = templates.get(template_key).format(name=name, device_twins_updates=device_twins_updates,
                                                                 device_twin_variables=device_twin_variables, type=type,
                                                                 context_type=context_type, lambda_code=lambda_property)
            except:
                print('ERROR: Problem formatting template "{template_key}". Check regular brackets in the template are escaped as {{{{.'.format(
                    template_key=template_key))
                continue

        hash_object = hashlib.md5(block_chars.encode())

        code_lines.append("\n")
        code_lines.append(
            '/// GENX_BEGIN ID:{name} MD5:{hash}\n'.format(name=name, hash=hash_object.hexdigest()))

        code_lines.append(templates[template_key].format(name=name, device_twins_updates=device_twins_updates,
                                                         device_twin_variables=device_twin_variables, type=type,
                                                         context_type=context_type, lambda_code=lambda_property))
        code_lines.append('\n/// GENX_END ID:{name}'.format(name=name))
        code_lines.append("\n\n")


def render_includes_block():

    if not os.path.isdir(generated_project_path + '/gx_includes'):
        os.mkdir(generated_project_path + '/gx_includes')

    for item in includes_block:

        block = includes_block.get(item)
        name = block.get('name')
        filename = generated_project_path + \
            '/gx_includes/gx_' + block.get('name') + '.h'

        if os.path.isfile(filename):
            continue

        template_key = block.get('include_template')

        if template_key is None or templates.get(template_key) is None:
            print('Key: "{template_key}" not found'.format(
                template_key=template_key))
            continue
        else:
            with open(filename, 'w') as include_file:
                include_file.write(templates.get(
                    template_key).format(name=name))

    with open(generated_project_path + '/gx_includes/gx_includes.h', 'w') as includes:
        includes.write('#pragma once\n\n')
        for name in includes_block:
            includes.write('#include "gx_' + name + '.h"\n')


def render_bindings(f):
    # bindings_tags = json.loads(templates.get('declare_bindings_tag'))
    bindings_tags = yaml.load(templates.get(
        'declare_bindings_tag'), Loader=yaml.FullLoader)

    for tag in bindings_tags:
        variable_list = ''

        var_list = binding_variables.get(tag, [])
        if len(var_list) == 0:
            if templates.get('declare_bindings_' + tag.lower()) is not None:
                f.write(templates.get('declare_bindings_' +
                        tag.lower()).format(variables=''))
            continue

        for var in var_list:
            if bindings_tags.get(tag) == None:
                continue
            variable_list += '&' + bindings_tags.get(tag) + '_' + var + ', '

        if variable_list == '':
            continue

        variable_list = variable_list[:-2]

        declare_binding = templates.get(
            'declare_bindings_' + tag.lower()).format(variables=variable_list)
        if declare_binding is not None:
            f.write(templates.get('declare_bindings_' +
                    tag.lower()).format(variables=variable_list))


def write_main():
    global device_twins_updates, device_twin_variables

    if not os.path.isdir(generated_project_path + '/gx_includes'):
        os.mkdir(generated_project_path + '/gx_includes')

    with open(generated_project_path + '/gx_includes/gx_bindings.h', 'w+') as df:

        df.write(templates["declarations"])

        render_signatures(df)

        df.write("\n")
        write_comment_block(df, 'Binding declarations')
        # render_timer_block(df)
        render_variable_block(df)
        df.write("\n")

        render_bindings(df)

        footer = templates["declarations_footer"]

        df.write(templates["declarations_footer"])

        render_handler_block()

        render_includes_block()

        with open(generated_project_path + "/main.c", "w") as mf:
            mf.writelines(code_lines)


def update_manifest():
    with open(generated_project_path + "/app_manifest.json", "w") as f:
        json.dump(manifest_updates, f, indent=2)


def load_main():
    global code_lines
    with open(generated_project_path + "/main.c", "r") as mf:
        code_lines = mf.readlines()

    clean = cleaner.Clean(code_lines)
    code_lines = clean.clean_main(handlers_block)


def validate_schema():
    pass


def init_stuff():
    pass


def process_update():
    init_stuff()
    load_bindings()
    load_templates()
    load_main()
    write_main()
    update_manifest()


process_update()


watch_file = 'app_model.yaml'

# also call custom action function
watcher = watcher.Watcher(watch_file, process_update)
watcher.watch()  # start the watch going
