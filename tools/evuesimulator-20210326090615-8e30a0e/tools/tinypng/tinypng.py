import os
import sys
import getopt
import tinify

"""
Document URL: https://tinify.cn/developers/reference/python

安装依赖：pip install -r requirements.txt

使用帮助：
    > python tinypng.py -i src -o target

参数解析：
    -i --input: 要输入的图片文件夹
    -o --output: 要输出的图片文件夹
"""

tinify.key = "7jprZ1sfcPPy2MrPwFwGY6b3HybbLZ6w"
# tinify.proxy = "http://user:pass@192.168.0.1:8080"

# 使用tinypng压缩图片
def compress_image(input, output):
    source = tinify.from_file(input)
    source.to_file(output)

    # with open("unoptimized.jpg", 'rb') as source:
    #     source_data = source.read()
    #     result_data = tinify.from_buffer(source_data).to_buffer()


# 获取指定目录下所有特定类型的图片
def search(root, target):
    result = []

    items = os.listdir(root)
    for item in items:
        path = os.path.join(root, item)
        filename, extension = os.path.splitext(path)
        if os.path.isdir(path):
            t = search(path, target)
            if len(t):
                result = result + t
                print('[-]', path)
        elif extension == target:
            result.append(filename + extension)
            print('[-]', path)
        elif path.split('/')[-1] == target:
            print('[+]', path)
        else:
            print('[!]', path)

    return result

def get_input_output_dir():
    """
    如果命令行需要通过linux那中指定参数名的方法运行，可以使用getopt()来获得参数内容

    命令【getopt.getopt(参数列表, "短选项参数", [长选项参数])】
    参数有两种维度区分：短选项，长选项；带参数值（开关）和不带参数值

    短选项参数（命令行使用-参数名）：由1个字母组成，如果字母后面跟冒号:表示这个参数需要有参数值，否则表示不输入参数值
        例如：io:
    表示入参要输入-i -o=xxx

    长选项参数（命令行使用--参数名）：由[参数名=]表示，有值的参数一定要写=，否则会抛错
        例如:["help","input=","output="]
    表示入参要输入 --help --input=xxx --output=xxx

    """

    opts, args = getopt.getopt(sys.argv[1:], "hi:o:", ["help", "input=", "output="])

    input_dir = ""
    output_dir = ""
    
    for opts, arg in opts:
        if opts == "-h" or opts == "--help":
            print("使用帮助：")
            print(">>>>>>>>>>")
            print("    > python tinypng.py -i src -o target")
            print("参数解析：")
            print("    -i --input: 要输入的图片文件夹src")
            print("    -o --output: 要输出的图片文件夹target")
        elif opts == "-i" or opts == "--input":
            input_dir = arg
        elif opts == "-o" or opts == "--output":
            output_dir = arg

    return input_dir, output_dir

if __name__ == "__main__":
    i, o = get_input_output_dir()

    if len(i) == 0 or len(o) == 0:
        raise "输入文件夹和输出文件都不能为空"

    if not os.path.isabs(i):
        i = os.path.join(os.getcwd(), i)

    if not os.path.isabs(o):
        o = os.path.join(os.getcwd(), o)

    if not os.path.exists(i):
        raise "输入目录：" + i + " 不存在"
        
    if not os.path.exists(o):
        raise "输出目录：" + o + " 不存在"

    dirs = search(os.path.join(os.path.dirname(os.getcwd()), "test/LiteWearable"), ".png")
    for d in dirs:
        folder_path, file_name = os.path.split(d)
        compress_image(d, os.path.join(o, file_name))