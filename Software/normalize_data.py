import sys
import xml.etree.ElementTree as ET
trans = str.maketrans('[]', '{}')

# Hệ số cố định để chuyển đổi float sang int
SCALE_FACTOR = 1000

def validate_args():
    if len(sys.argv) != 2:
        print(f"Invalid number of arguments ({len(sys.argv)}). Usage: python xml_float63toint32.py haarcascade_frontalface_default.xml")
        sys.exit(-1)

    xml_file_path = sys.argv[1]
    if not xml_file_path.endswith(".xml"):
        print("Invalid argument: This script only works on files ending in .xml")
        sys.exit(-1)
    
    return xml_file_path

def parse_xml(xml_file_path):
    tree = ET.parse(xml_file_path)
    return tree.getroot()

def create_c_file(filename):
    try:
        with open(filename, 'w') as f:
            f.write("// Auto-generated C file from XML\n#include \"../source/detectFace.h\"\n\n")
        print(f"C file created: {filename}")
    except Exception as e:
        print(f"Error: Could not create file: {e}")
        sys.exit(0)

def write_to_c_file(filename, content):
    try:
        with open(filename, 'a') as f:
            f.write(content)
    except Exception as e:
        print(f"Error: Could not write to file: {e}")
        sys.exit(0)

def scale_to_int(value):
    """Chuyển đổi float sang int bằng cách nhân với SCALE_FACTOR."""
    return int(value * SCALE_FACTOR)

def extract_cascade_params(root):
    height = int(root.find("./cascade/height").text)
    width = int(root.find("./cascade/width").text)
    max_stage_size = int(root.find("./cascade/stageParams/maxWeakCount").text)
    stage_num = int(root.find("./cascade/stageNum").text)
    
    return height, width, max_stage_size, stage_num

def extract_stage_data(root):
    stage_thresholds = []
    stage_organization = []
    nodes = []
    
    for stage in root.findall("./cascade/stages/"):
        stage_thresholds.append(scale_to_int(float(stage.find('stageThreshold').text)))
        stage_organization.append(int(stage.find('maxWeakCount').text))
        
        for w_classifier in stage.findall("./weakClassifiers/_"):
            internal_nodes = w_classifier.find('internalNodes').text.strip().split(' ')
            leaf_values = w_classifier.find('leafValues').text.strip().split(' ')
            
            feat_index = int(internal_nodes[2])
            node_thresh = scale_to_int(float(internal_nodes[3]))
            l_val = scale_to_int(float(leaf_values[0]))
            r_val = scale_to_int(float(leaf_values[1]))
            
            nodes.extend([node_thresh, l_val, r_val])
    
    return stage_thresholds, stage_organization, nodes

def extract_rectangles(root):
    feat_organization = []
    rectangles = []
    total_rect_count = 0
    
    for feature in root.findall("./cascade/features/"):
        rect_count_in_feature = 0
        for rect in feature.findall("./rects/"):
            rect_values = [int(float(val)) for val in rect.text.strip().split(' ')]
            rectangles.append(rect_values)
            rect_count_in_feature += 1
            total_rect_count += 1
        
        feat_organization.append(rect_count_in_feature)
    
    return feat_organization, rectangles, total_rect_count

def main():
    xml_file_path = validate_args()
    root = parse_xml(xml_file_path)
    
    c_file_name = xml_file_path.replace(".xml", "5.c")
    create_c_file(c_file_name)
    
    height, width, max_stage_size, stage_num = extract_cascade_params(root)
    
    write_to_c_file(c_file_name, f"const int height = {height};\n")
    write_to_c_file(c_file_name, f"const int width = {width};\n")
    write_to_c_file(c_file_name, f"const int maxStageSize = {max_stage_size};\n")
    write_to_c_file(c_file_name, f"const int stageNum = {stage_num};\n")
    write_to_c_file(c_file_name, f"const int SCALE_FACTOR = {SCALE_FACTOR};\n")
    
    stage_thresholds, stage_organization, nodes = extract_stage_data(root)
    
    write_to_c_file(c_file_name, f"\nconst int stageOrga[{len(stage_organization)}] = {str(stage_organization).translate(trans)};\n")
    write_to_c_file(c_file_name, f"\nconst int stageThresholds[{len(stage_thresholds)}] = {str(stage_thresholds).translate(trans)};\n")
    write_to_c_file(c_file_name, f"\nconst int nodes[{len(nodes)}] = {str(nodes).translate(trans)};\n")
    
    
    feat_organization, rectangles, total_rect_count = extract_rectangles(root)
    
    write_to_c_file(c_file_name, f"\nconst int featOrga[{len(feat_organization)}] = {str(feat_organization).translate(trans)};\n")
    write_to_c_file(c_file_name, f"\nconst int rectangles[{total_rect_count}][5] = {str(rectangles).translate(trans)};\n")
    
    print(f"C file generated: {c_file_name}")

if __name__ == "__main__":
    main()
