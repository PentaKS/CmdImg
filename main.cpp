#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

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
void greyscale_image() {
    for (auto & p : image_data) {
        // double max_of_three = fmax(p[R], p[G]);
        // max_of_three = fmax(p[B], max_of_three);    //// best for normal images 

        // double sq_avg = (p[R]*p[R] + p[G]*p[G] + p[B]*p[B])/3;
        // int maxv = int (sqrt(sq_avg)); //// best for really bright images 

        double avg = (p[R] + p[G] + p[B])/3;
        int maxv = int (avg); //// best for really bright images 
        
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
string subpixel_black_shadows = "`'\".,~!()/\\|[]";
string subpixel_green_black = "`'.,~!()/\\|[]";
//////////////
std::vector<string> ascii_gradients = {ascii_gradient1, ascii_gradient2, box_gradient, box_gradient2, visual_density, subpixel,subpixel_black_shadows,subpixel_green_black};
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
    string input_path_ppm = argsv[1];
    string command_sep = argsv[2];
    string output_path_ppm;
conversion_label:
    if (verify_path(input_path_ppm) == true) {
        cout << "path :" << input_path_ppm << endl;
        load_image_data(input_path_ppm);
        char command = 'c';
        int console_height = 0;
        int scaling_factor = 1;

        loop_label:
            cout << "\nenter your max console height(h) \nor \nenter your downscale factor(d) [current: " << num_width << "X" <<  num_height <<"]  (h/d): ";
            cin >> command;
            if ('h' == command) {
                cout << "enter max height (int) : ";
                cin >> console_height;
                scaling_factor = num_height/console_height;
            } else if ('d' == command) {
                cout << "enter downscale factor (int): ";
                cin >> scaling_factor;
            } else {
                cout << "invalid option" << endl;
                goto loop_label;
            }
        greyscale_image();

        image_data = scale_down_image(image_data, scaling_factor);

        int m;
        string mode_command = argsv[4];
        if (mode_command == "-mode" || mode_command == "-m") {
            string mode = argsv[5];
            stringstream mstream(mode);
            mstream >> m;
        } else {
            m = 0;
            std::cout << "no mode is given [switching to default mode] i.e. -m 0";
        }

        if (m < ascii_gradients.size() && m >= 0)
            print_ascii(image_data, m);
        else 
            cout << "[invalid mode] can't generate ASCII, available modes are integers in [0, " << ascii_gradients.size()-1 << "] " << endl;

        if (command_sep == "-o" || command_sep == "-output") {
            cout << "\noutput path detected" <<endl;
            output_path_ppm = argsv[3];
            if (verify_path(output_path_ppm) == true) {
                write_image_data(output_path_ppm);
            } else {
                cout << "invalid output path" << endl;
            }
        } else {
            cout << "overwriting otiginal image" << endl;
            output_path_ppm = input_path_ppm;
            write_image_data(output_path_ppm);
        }


    } else {
        ////// code doesn't work
        cout << "converting input image to ppm" << endl;
        // string converted_output = input_path_ppm;
        // converted_output = converted_output.substr(0, converted_output.size() - 5);
        // converted_output = converted_output+"_converted.ppm";
        // string command_convert = "ffmpeg -i "+input_path_ppm+" "+converted_output;
        // system (command_convert.c_str());

        // input_path_ppm = converted_output;
        // goto conversion_label;

    }
}