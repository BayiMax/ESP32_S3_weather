/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-20 20:15:21
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-31 18:50:26
 * @FilePath: \esps3_V1\components\WIFI\include\Serve_Http.h
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#ifndef SERVE_HTTP_H
#define SERVE_HTTP_H
/*



<script>\
function myrefresh(){window.location.reload();}\
setTimeout('myrefresh()',10000);\
</script>\

<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0\">\
<meta name=\"format-detection\" content=\"telephone=no\">\

*/
char User_HTTP_Front[] = {"<html>\
<head>\
<title>Bay_Max-data</title>\
<meta http-equiv=\"refresh\" content=\"0.500\">\
<style>\
.div_all{background-color:rgb(226,218,207);}\
.div_top{background-color:rgb(232,236,229);\
text-align:center;\
color:rgb(14,87,87);\
font-size:20px;}\
.div_data{\
background-color:rgb(221,239,233);\
font-size:20px;\
position:absolute;\
left:10%%;}\
</style>\
</head>\
<body>\
<div class=\"div_all\">\
<div class=\"div_top\">\
-ESP数据显示页面-\
</div>\
<div class=\"div_data\">"};

char User_HTTP_Queen[] = {"</div></div></body></html>"};

#endif /* SERVE_HTTP_H */
