'''
Author: baymax 2978043737@qq.com
Date: 2022-10-17 21:02:26
LastEditors: baymax 2978043737@qq.com
LastEditTime: 2022-10-18 00:26:50
FilePath: \esps3_V1\example_test.py
Description: 

Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved. 
'''
#!/usr/bin/env python

from __future__ import division, print_function, unicode_literals

import ttfw_idf

@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3'], ci_target=['esp32'])

def test_examples_hello_world(env, extra_data):
    app_name = 'esps3_v1'
    dut = env.get_dut(app_name, 'examples/get-started/hello_world')
    dut.start_app()
    res = dut.expect(ttfw_idf.MINIMUM_FREE_HEAP_SIZE_RE)
    if not res:
        raise ValueError('Maximum heap size info not found')
    ttfw_idf.print_heap_size(app_name, dut.app.config_name, dut.TARGET, res[0])

if __name__ == '__main__':
    test_examples_hello_world()
