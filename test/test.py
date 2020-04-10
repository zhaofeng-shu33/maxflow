# use subprocess
# to invoke each method
# and compare the result
import subprocess
import os

def get_result(method):
    cmd = ['./maxflow', method, '-f', 'example.inp']
    output_obj = subprocess.run(cmd,
                 cwd=os.getcwd(),
                 stdout=subprocess.PIPE)
    return output_obj.stdout.decode('utf-8')

def check_result(string):
    flow_line = string.split('\n')[2]
    flow_value = flow_line.split('\t\t')[1]
    if int(flow_value) != 11:
        raise ValueError(flow_line)
 
if __name__ == '__main__':
    method_list = ['ek', 'din', 'prf', 'prh',
                   'ppr', 'prs', 'ao', 'aos']
    for method in method_list:
        result_str = get_result(method)
        check_result(result_str)
    print("All checks have passed")
