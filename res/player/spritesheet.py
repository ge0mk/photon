#!/bin/python3

import os
import json
import re
from PIL import Image
from math import *

def next_power_of_2(n):
    if n == 0:
        return 1
    if n & (n - 1) == 0:
        return n
    while n & (n - 1) > 0:
        n &= (n - 1)
    return n << 1

def fix_json_indent(text, indent=3):
    space_indent = indent * 4
    initial = " " * space_indent
    json_output = []
    current_level_elems = []
    all_entries_at_level = None  # holder for consecutive entries at exact space_indent level
    for line in text.splitlines():
        if line.startswith(initial):
            if line[space_indent] == " ":
                # line indented further than the level
                if all_entries_at_level:
                    current_level_elems.append(all_entries_at_level)
                    all_entries_at_level = None
                item = line.strip()
                current_level_elems.append(item)
                if item.endswith(","):
                    current_level_elems.append(" ")
            elif current_level_elems:
                # line on the same space_indent level
                # no more sublevel_entries
                current_level_elems.append(line.strip())
                json_output.append("".join(current_level_elems))
                current_level_elems = []
            else:
                # line at the exact space_indent level but no items indented further
                if all_entries_at_level:
                    # last pending item was not the start of a new sublevel_entries.
                    json_output.append(all_entries_at_level)
                all_entries_at_level = line.rstrip()
        else:
            if all_entries_at_level:
                json_output.append(all_entries_at_level)
                all_entries_at_level = None
            if current_level_elems:
                json_output.append("".join(current_level_elems))
            json_output.append(line)
    return "\n".join(json_output)

sprites = dict()
framecount = 0

maxw = 0
maxh = 0

for file in os.listdir("./sprites"):
    if(file.endswith(".png")):
        animation = file[0:-7].replace("adventurer-", "")
        index = int(file[-6:-4])
        if(animation not in sprites):
            sprites[animation] = dict()
        sprites[animation][str(index)] = file
        framecount += 1

        img = Image.open("./sprites/" + file)
        maxw = max(maxw, img.size[0])
        maxh = max(maxh, img.size[1])

cols = next_power_of_2(int(ceil(sqrt(framecount))))
rows = int(ceil(float(framecount) / float(cols)))

w = cols * maxw
h = rows * maxh

atlas = Image.new('RGBA', (w, h), (0, 0, 0, 0))

r = 0
c = 0

for name, animation in sorted(sprites.items()):
    uvs = list()
    for index, frame in sorted(animation.items()):
        current = Image.open("./sprites/" + frame)
        px = c * maxw
        py = r * maxh

        uvs += [[c, r]]

        atlas.paste(current, (px, py))

        c += 1
        if c == cols:
            c = 0
            r += 1

    sprites[name]["uvs"] = uvs

atlas.save("atlas.png")

out = json.dumps(sprites, sort_keys=True, indent=4)

out = fix_json_indent(out, 3)

with open("sprites.json", "w") as f:
    f.write(out)

with open("anim.cpp", "w") as f:
    f.write("enum Animation : uint8_t {\n")
    index = 0
    for animname in sorted(sprites.keys()):
        enumname = "a_" + animname.replace("-", "_")
        f.write("\t" + enumname + ",\n")
        index += 1
    f.write("};\n")

    f.write("\n")

    f.write("const std::vector<std::vector<ivec2>> Player::animations = {\n")
    for animname in sorted(sprites.keys()):
        line = ""
        for uv in sprites[animname]["uvs"]:
            x = uv[0]
            y = uv[1]
            line += "ivec2(" + str(x) + ", " + str(y) + "), "
        f.write("\t{" + line[:-2] + "},\n")
    f.write("};\n")