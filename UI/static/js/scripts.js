var http = new XMLHttpRequest()
var url = "/new"
var params = "name=foobar"
http.open("POST", url, true)

http.setRequestHeader("Content-length", params.length);
http.setRequestHeader("Connection", "close");

http.onreadystatechange = function() {
	if(http.readyState == 4 && http.status == 200) {
		alert(http.responseText);
	}
}
http.send(params);
