#pragma once 
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
using namespace std;



std::vector<string> string_input_commands = {
    "-i",
    "-input",
    "-o",
    "-output",
    "-lnm",
    "-load_new_mode",
    "-ot",
    "-out_txt"
};
std::vector<string> int_input_commands = {
    "-m",
    "-mode",
    "-g",
    "-greyscale_mode",
    "-s",
    "-shades",
    "-ch",
    "-console_height",
    "-cw",
    "-console_width",
    "-d",
    "-downscale_factor"
};
std::vector<string> no_input_commands = {
    "-h",
    "-help",
    "-na",
    "-no_ascii"
};

std::vector<string> blocks;
void load_blocks (int argsc, char * argsv[]) {
    for (int i = 1; i < argsc; i++) {
        blocks.push_back(string(argsv[i]));
    }
}
std::map<string, string> parse_string_arguments () {
    std::map<string, string> out_strings;
    int iterator = 0;
    for (auto & b: blocks) {
        for (auto & c: string_input_commands) {
            if (b == c) {
                out_strings[b] = blocks[iterator+1];
            }
        }

        iterator++;
    }

    return out_strings;
}

std::map<string, int> parse_int_arguments () {
    std::map<string, int> out_integers;
    int iterator = 0;
    for (auto & b: blocks) {
        for (auto & c: int_input_commands) {
            if (b == c) {
                stringstream int_block(blocks[iterator+1]);
                int output = 0;
                int_block >> output;

                out_integers[b] = output;
            }
        }

        iterator++;
    }

    return out_integers;
}

std::vector<string> parse_no_input_arguments () {
    std::vector<string> out_commands;
    int iterator = 0;
    for (auto & b: blocks) {
        for (auto & c: no_input_commands) {
            if (b == c) {
                out_commands.push_back(b);
            }
        }

        iterator++;
    }

    return out_commands;
}

void show_help () {
    cout << endl;
    cout << "-------- Commands -------------------------- Usage --------------" << endl << endl;
    cout << "        -i / -input       | input image  e.g. -i test_image_1.ppm" << endl;
    cout << "        -o / -output      | output image e.g. -o output.ppm      " << endl;
    cout << "       -ot / -out_txt     | output the ascii values generated to " << endl;
    cout << "                          | a given path e.g -ot out.txt         " << endl;
    cout << "        -m / -mode        | ascii mode / style, available modes: " << endl;
    cout << "                          |     [-m 0] ascii_gradient1           " << endl;
    cout << "                          |     [-m 1] ascii_gradient2           " << endl;
    cout << "                          |     [-m 2] box_gradient1             " << endl;
    cout << "                          |     [-m 3] box_gradient2             " << endl;
    cout << "                          |     [-m 4] visual_density            " << endl;
    cout << "                          |     [-m 5] subpixel                  " << endl;
    cout << "                          |     [-m 6] subpixel_black_shadows    " << endl;
    cout << "                          |     [-m 7] subpixel_green_black      " << endl;
    cout << "                          |     e.g. -m 3                        " << endl;
    cout << "  -lnm / -load_new_mode   | can load custom ascii gradients from " << endl;
    cout << "                          | a .txt file.                         " << endl;
    cout << "                          | * the file should be of the form:    " << endl;
    cout << "                          |      gradient_name = [gradient]      " << endl;
    cout << "                          | e.g. my_mode1 = `;!%@                " << endl;
    cout << "                          | example: -lnm my_mode.txt            " << endl;
    cout << "   -ch / -console_height  | images are larger than console,      " << endl;
    cout << "                          | automatic scaling_factor calculation " << endl;
    cout << "                          | form console height. e.g. -ch 40     " << endl;
    cout << "   -cw / -console_width   | images are larger than console,      " << endl;
    cout << "                          | automatic scaling_factor calculation " << endl;
    cout << "                          | form console width   e.g. -cw 40     " << endl;
    cout << "   -d / -downscale_factor | images are larger than console,      " << endl;
    cout << "                          | manual scaling_factor input          " << endl;
    cout << "                          | e.g. -d 4                            " << endl;
    cout << "     -na / -no_ascii      | if don't want to print image as ascii" << endl;
    cout << "      -h / -help          | provide all commands and instructions" << endl;
    cout << "-----------------------------------------------------------------" << endl;

}

