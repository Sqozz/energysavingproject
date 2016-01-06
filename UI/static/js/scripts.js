function switchPlug(plugName, state) {
	var http = new XMLHttpRequest()
	var url = "/switchPlug"
	var params = "newState=" + state + "&plugName=" + plugName
	http.open("POST", url, true)

	http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	http.setRequestHeader("Content-length", params.length);
	http.setRequestHeader("Connection", "close");

	http.onreadystatechange = function() {
		if(http.readyState == 4 && http.status == 200) {
		}
	}
	http.send(params);
}
