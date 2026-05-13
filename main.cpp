#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include "command_manager.hpp"

using namespace std;
typedef std::vector<std::vector<int>> IMG_DATA;

constexpr int R = 0, G = 1, B = 2;

bool verify_path(string &path) {
    int path_size = path.size();
    if (path.substr(path_size - 3, 3) == "ppm") {
        cout << "verified file format" << endl;
        return true;
    } else {
        cout << "invalid file format, expected format (.ppm)" << endl;
        return false;
    }
    return true;
}
string Header, Width, Height, RGB; // header data
int num_width = 0, num_height = 0; // numeric conversion
int num_width_c = 0, num_height_c = 0; // numeric copy
IMG_DATA image_data; // pixel data
IMG_DATA image_color_data; // pixel color;

void load_image_data (string i_path) {
    ifstream image(i_path);
    
    if (image.is_open()) {
        image >> Header >> Width >> Height >> RGB;
        stringstream w(Width), h(Height);
        w >> num_width;
        h >> num_height;
        num_height_c = num_height;
        num_width_c = num_width;
    } else {
        cout << "image does not exist" <<endl;
    }

    
    string red, green, blue;
    while(!image.eof()) {
        image >> red >> green >> blue;
        int r = 0, g = 0, b = 0;
        stringstream rs(red), gs(green), bs(blue);
        rs >> r;
        gs >> g;
        bs >> b;

        std::vector<int> pixel = {r, g, b};
        image_data.push_back(pixel);

        int maxv = fmax(r, g);
        maxv = fmax(maxv, b);
        char color = '.';
        if (maxv == r) color = ']';
        else if (maxv == g) color = '`';
        else if (maxv == b) color = '[';

        std::vector<char> colors = {color};
    }

    cout << "successfully loaded" << endl;
}

void write_image_data (string o_path, int o_width = num_width_c, int o_height = num_height_c) {
    ofstream output(o_path);

    if (output.is_open()) {
        output << Header << endl; 
        output << o_width << endl; 
        output << o_height << endl; 
        output << RGB << endl;
    } else {
        cout << "output can not be formed" << endl;
    }

    for (auto & p : image_data) {
        output << p[R] << " " << p[G] << " " << p[B] << endl;
    }

    cout << "check your file "<<o_width << "X" << o_height << " : " << o_path << endl;
}
void greyscale_image(int greyscale_mode=0, int shades = 255) {

    for (auto & p : image_data) {
        int maxv = 0;

        switch (greyscale_mode) {
            case 0:{
                double avg = (p[R] + p[G] + p[B])/3;
                maxv = int (avg);
                break;
            }
            case 1:{
                double max_of_three = fmax(p[R], p[G]);
                maxv = fmax(p[B], max_of_three);
                break;
            }
            case 2:{
                double sq_avg = (p[R]*p[R] + p[G]*p[G] + p[B]*p[B])/3;
                maxv = int (sqrt(sq_avg));
                break;
            }
            case 3:{
                double avg = (p[R] + p[G] + p[B])/3;
                double min_shade = 255/ (shades -1);
                maxv = int((avg/min_shade) + 0.5) * min_shade;
                break;
            }
        }
        // double max_of_three = fmax(p[R], p[G]);
        // max_of_three = fmax(p[B], max_of_three);    //// best for normal images 

        // double sq_avg = (p[R]*p[R] + p[G]*p[G] + p[B]*p[B])/3;
        // int maxv = int (sqrt(sq_avg)); //// best for really bright images 

        // double avg = (p[R] + p[G] + p[B])/3;
        // int maxv = int (avg); //// best for really bright images 
        
        p = {maxv, maxv, maxv};
    }

    cout << "greyscale filter applied" << endl;
}

IMG_DATA crop_image(int boxpx, int boxpy, int boxw, int boxh, IMG_DATA input_image_data) {
    cout << "box dimensions: " << boxw << "X" << boxh << endl;
    IMG_DATA new_image_data;
    int iterator = 0;
    int pixel_y = boxpy;
    for (auto & p : input_image_data) {
        if (iterator < boxpx + boxw + num_width*pixel_y && iterator >= boxpx + num_width*pixel_y) {
            new_image_data.push_back(p);
        }
        if (iterator == boxpx + boxw + num_width*pixel_y ){ pixel_y++;}

        iterator++;
    }
    cout << "cropped succesfully" << endl;

    return new_image_data;

}

IMG_DATA scale_down_image (IMG_DATA input_image_data, int scaling_factor = 1) {
    IMG_DATA new_image_data;
    int iterator = 0;
    for (auto & p : input_image_data) {
        if (iterator % scaling_factor == 0) {
            new_image_data.push_back(p);
        }

        iterator++;
    }

    num_width_c =  int(num_width_c/scaling_factor); num_height_c = int(num_height_c / scaling_factor);

    cout << "scaled down into "<< scaling_factor << " images, cropping the first one" << endl;

    if (scaling_factor != 1)
        new_image_data =  crop_image(0, 0, num_width_c, num_height_c, new_image_data);
    return new_image_data;
}

string ascii_gradient1 = "`.,-:;~'\"^*<+>(){}[]!$&@#%"; // best for normal images
string ascii_gradient2 = "`.,'\"::^-+~!={}()[]*&$#@%";
string box_gradient = ".,:;-~+=![]";  // best for bright images
string box_gradient2 = "`-/.\\[]!#";
/// Ai suggestions
string visual_density = " ,;-=+*#%@";  // most beautiful 
string subpixel = "`'\".,~!()/\\|[]";
//////////////
string subpixel_black_shadows = "`'\".,~!()/\\|[]";
string subpixel_green_black = "`'.,~!()/\\|[]";
std::vector<string> ascii_gradients = {ascii_gradient1, ascii_gradient2, box_gradient, box_gradient2, visual_density, subpixel,subpixel_black_shadows,subpixel_green_black};

void load_new_mode (string filename) {
    ascii_gradients.clear();

    ifstream mode(filename);
    string modename, equals_to, gradient;
    std::vector<string> modenames;

    while(!mode.eof()) {
        mode >> modename >> equals_to >> gradient;
        ascii_gradients.push_back(gradient);
        modenames.push_back(modename);
    }

    cout << "new modes loaded from: " << filename << " they are:" << endl;
    int iterator = 0;
    for (auto & m: modenames) {
        cout << "["<< iterator<< "]  " << m << endl;
        iterator++;
    }

    cout << "Use these integers to select mode when asked" << endl;
}
char detect_value(int val, int ascii_gradient_mode) {
    auto ascii_gradient = ascii_gradients[ascii_gradient_mode];
    int gs = ascii_gradient.size();
    for (int i = 0; i < gs; i++)
    {
        if (val >= (255 / gs-2) * (i-1) && val < (255 / gs-2) * (i))
        {
            return ascii_gradient[i];
        }
    }
    return ascii_gradient[gs-1];
}

void print_ascii(IMG_DATA final_image, int ascii_gradient_mode) {
    int iterator = 0;
    for (auto & p : final_image) {
        cout << detect_value(p[R], ascii_gradient_mode) << " ";

        if (iterator % num_width_c == 0) {
            cout << endl;
        }
        iterator ++;
    }
}

int main (int argsc, char* argsv[]) {

    string input_path_ppm = "default";
    string output_path_ppm = "default";
    string load_new_mode_path_txt = "default";

    load_blocks(argsc, argsv);
    auto string_arguments = parse_string_arguments();
    for (auto & sa : string_arguments) {
        if (sa.first == "-i" || sa.first == "-input") {
            input_path_ppm = sa.second;
        } else if (sa.first == "-o" || sa.first == "-output") {
            output_path_ppm = sa.second;
        } else if (sa.first == "-lnm" || sa.first == "-load_new_mode") {
            auto copy = ascii_gradients;
            load_new_mode_path_txt = sa.second;
            load_new_mode(load_new_mode_path_txt);
            if (ascii_gradients.size() == 0) {
                cout << "nothing in the given mode file switching to default modes [0,7]" << endl;
                ascii_gradients = copy;
            }
        }
    }

    int mode = 0, greyscale_mode = 0, console_height = 0, console_width = 0, downscale_factor = 1;
    int number_of_shades = 255;
    auto int_arguments = parse_int_arguments();
    for (auto & ia : int_arguments) {
        if (ia.first == "-m" || ia.first == "-mode") {
            mode = ia.second;
        } else if (ia.first == "-g" || ia.first == "-greyscale_mode") {
            greyscale_mode = ia.second;
        } else if (ia.first == "-ch" || ia.first == "-console_height") {
            console_height = ia.second;
        } else if (ia.first == "-cw" || ia.first == "-console_width") {
            console_width = ia.second;
        } else if (ia.first == "-d" || ia.first == "-downscale_factor") {
            downscale_factor = ia.second;
        } else if (ia.first == "-s" || ia.first == "-shades") {
            number_of_shades = ia.second;
        }
    }
    cout << "mode " << mode << " greyscale_mode " << greyscale_mode << " console_height" << console_height << " console_width " << console_width << " downscale_factor " << downscale_factor << " shades " << number_of_shades << endl;

    string help = "default", no_ascii = "false";
    auto no_input_arguments = parse_no_input_arguments();
    for (auto & nia : no_input_arguments) {
        if ("-help" == nia || "-h" == nia) {
            show_help();
            goto end_of_program;
        } else if ("-no_ascii" == nia || "-na" == nia) {
            no_ascii = "true";
            cout << "ascii changed";
        }
    }

    if (verify_path(input_path_ppm) == true) {
        cout << "input path detected. path : "<< input_path_ppm << endl;
        load_image_data(input_path_ppm);

        if (console_height == 0 && console_width == 0 && downscale_factor == 1) {
            string response = "default";
            ask_again:
                cout << "you have not specified any of the [console_height/console_width/downscale_factor] or keep downscale_factor = 1 ? \nplease specify [cw/ch/d/keep]: ";
                cin >> response;
                if ("ch" == response || "1" == response || "console_height" == response) {
                    cout << "please enter console_height: ";
                    cin >> console_height;
                } else if ("cw" == response || "2" == response || "console_width" == response) {
                    cout << "please enter console_width: ";
                    cin >> console_width;
                } else if ("d" == response || "3" == response || "downscale_factor" == response) {
                    cout << "please enter downscale_factor: ";
                    cin >> downscale_factor;
                } else if ("keep" == response || "4" == response || "k" == response) {
                    downscale_factor = 1;
                } else {
                    goto ask_again;
                }
        } else {
            cout << "invalid path" << endl;
        }

        int scaling_factor = 1;
        if (console_height!=0) {
            scaling_factor = num_height/console_height;
        } else if (console_width!=0) {
            scaling_factor = num_width/console_width;
        } else {
            scaling_factor = downscale_factor;
        }

        image_data = scale_down_image(image_data, scaling_factor);

        if (greyscale_mode < 4 && greyscale_mode >= 0 && number_of_shades>=2 && number_of_shades <= 255)
            greyscale_image(greyscale_mode, number_of_shades);
        else {
            cout << "shades can't be zero or greater than 255 and greyscale modes are in range [0, 3]" << endl;
            goto end_of_program;
        }

        if (mode == 0) {
            cout << "no mode specified, current m = 0" << endl;
            char response = 'c';
            ask_mode:
                cout << "do you want to enter a new mode [y/n]: ";
                cin >> response; 
                if ('y' == response) {
                    cout << "enter new mode(int): ";
                    cin >> mode;
                } else if ('n' == response) {
                    cout << "mode is : 0" << endl;
                } else {
                    goto ask_mode;
                }
        }
        cout << no_ascii << endl;
        if (no_ascii == "true") {
            cout << "not printing ascii as specified" << endl;
        } else if (mode >=0 && mode < ascii_gradients.size()) {
            print_ascii(image_data, mode);
        } else {
            cout << "[invalid mode] can't generate ASCII, available modes are integers in [0, " << ascii_gradients.size()-1 << "] " << endl;
        }
        
        if (output_path_ppm != "default") {
            if(verify_path(output_path_ppm) == true) {
                cout << "output path detected. writing to: " << output_path_ppm << endl;
                write_image_data(output_path_ppm);
            } else {
                cout << "invalid output path." << endl;
            }
        } else {
            cout << "\nno output path specified. " << endl;
            char yes_or_no = 'c';
            ask_about_out_path_again:
                cout << "overwrite the current image path or save it [y/n/s]?: ";
                cin >> yes_or_no;
                if ('y' == yes_or_no || 'Y' == yes_or_no) {
                    write_image_data(input_path_ppm);
                    cout << "check image: " << input_path_ppm << endl;
                } else if ('n' == yes_or_no || 'N' == yes_or_no) {
                    cout << "image not saved" << endl;
                } else if ('s' == yes_or_no || 'S' == yes_or_no) {
                    cout << "enter the output path: " ;
                    cin >> output_path_ppm;
                    write_image_data(output_path_ppm);
                    cout << "check image: " << output_path_ppm << endl;
                } else {
                    goto ask_about_out_path_again;
                }
        }
    }

    end_of_program:
    cout << "program terminated" <<endl;

}