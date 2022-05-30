
//var websocket = new WebSocket('ws://'+location.hostname+'/');
//websocket.onopen = onOpen;
//websocket.onclose = onClose;
//websocket.onmessage = onMessage;
window.addEventListener('load', onload);

// Get the element with id="defaultOpen" and click on it
document.getElementById("defaultOpen").click();

function onload(event) 
{
	document.getElementById("connectionStatusLabel").innerHTML = "Disconnected";
	document.getElementById("counterLabel").innerHTML = "0";
	
	document.getElementById("panHeading").value = "";
	document.getElementById("panOffset").value = "";
	document.getElementById("panManualHeadingSetBtn").disabled = true;
	document.getElementById("panManualSwitch").checked = false;
	document.getElementById("panManualHeading").value = "";
	document.getElementById("panManualSlider").value = 0;
	document.getElementById("panManualHeading").disabled = true;
	document.getElementById("panManualSlider").disabled = true;
	
	initWebSocket();
}

function openPage(pageName, elmnt, color) 
{
	// Hide all elements with class="tabcontent" by default */
	var i, tabcontent, tablinks;
	tabcontent = document.getElementsByClassName("tabcontent");
	for (i = 0; i < tabcontent.length; i++) 
	{
		tabcontent[i].style.display = "none";
	}

	// Remove the background color of all tablinks/buttons
	tablinks = document.getElementsByClassName("tablink");
	for (i = 0; i < tablinks.length; i++) 
	{
		tablinks[i].style.backgroundColor = "";
	}

	// Show the specific tab content
	document.getElementById(pageName).style.display = "block";

	// Add the specific color to the button used to open the tab content
	elmnt.style.backgroundColor = color;
}

function initWebSocket() 
{
	try {
		websocket = new WebSocket('ws://'+location.hostname+'/');
	} catch (e) {
		setTimeout(initWebSocket, 2000);
	}
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) 
{
	document.getElementById("connectionStatusLabel").innerHTML = "Connected";
    requestValues();
}

function onClose(event) 
{
	document.getElementById("connectionStatusLabel").innerHTML = "Disconnected";
	setTimeout(initWebSocket, 2000);
}

function onMessage(event) 
{
    var msg = event.data;
	console.log(msg);
	var obj = JSON.parse(msg);
	var param;
	
	param = "frame";
	if (obj.hasOwnProperty(param))
	{
		document.getElementById("counterLabel").innerHTML = obj[param];
	}
	
	param = "pan_state";
	if (obj.hasOwnProperty(param))
	{
		document.getElementById("panState").value = obj[param];
	}
	
	param = "panOffset";
	if (obj.hasOwnProperty(param))
	{
		document.getElementById(param).value = obj[param];
	}
	
	param = "pan_manual_mode";
	if (obj.hasOwnProperty(param))
	{
		if (parseInt(obj[param]) == 0)
		{
			document.getElementById("panManualSwitch").checked = false;
			// TODO - disable other controls
		}
		else
		{
			document.getElementById("panManualSwitch").checked = true;
			// TODO - disable other controls
		}
	}
	
	param = "pan_manual_heading";
	if (obj.hasOwnProperty(param))
	{
		document.getElementById("panManualHeading").value = obj[param];
	}
	
/*
	switch(msg.charAt(0)) 
	{
		case 'L':
			console.log(msg);
			var value = parseInt(msg.replace(/[^0-9\.]/g, ''), 10);
			document.getElementById("slider1").value = value;
			break;
		case 'C':
			document.getElementById("counterLabel").innerHTML = msg;
			break;
	} */
}


function requestValues()
{
	requestPanParameters();
}


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//                                PAN                                        //
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

function onPanOffsetSetBtnClicked(element) 
{
	var text = document.getElementById("panOffset").value;
	var value = parseInt(text, 10);
	var obj = {pan_offset: value.toString()};	
    var jsonstr = JSON.stringify(obj);
	console.log(jsonstr);
	websocket.send(jsonstr);
}

function onPanManualModeChanged(element)
{
	var isManualMode = element.checked;
	if (isManualMode)
	{
		// TODO CSS disabled state for slider
		document.getElementById("panManualHeading").disabled = false;
		document.getElementById("panManualSlider").disabled = false;
		document.getElementById("panManualHeadingSetBtn").disabled = false;
	}
	else
	{
		document.getElementById("panManualHeading").disabled = true;
		document.getElementById("panManualSlider").disabled = true;
		document.getElementById("panManualHeadingSetBtn").disabled = true;
	}
	var obj = {pan_manual_mode: isManualMode.toString()};	
	var jsonstr = JSON.stringify(obj);
	console.log(jsonstr);
	websocket.send(jsonstr);
}

function onPanManualHeadingSetBtn(element)
{
	var text = document.getElementById("panManualHeading").value;
	var value = parseInt(text, 10);
	document.getElementById("panManualSlider").value = value;
	var obj = {pan_manual_heading: value.toString()};	
	var jsonstr = JSON.stringify(obj);
	console.log(jsonstr);
	websocket.send(jsonstr);
}

function onPanManualSliderChanged(element) 
{
    var value = element.value;
	document.getElementById("panManualHeading").value = value;
	var obj = {pan_manual_heading: value.toString()};	
	var jsonstr = JSON.stringify(obj);
	console.log(jsonstr);
	websocket.send(jsonstr);
}

function requestPanParameters()
{
	var obj= {};
	obj["pan_state"] = "?";
	obj["pan_offset"] = "?";
	obj["pan_manual_mode"] = "?";	
	obj["pan_manual_heading"] = "?";
	var jsonstr = JSON.stringify(obj);
	console.log(jsonstr);
	websocket.send(jsonstr);
}


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//                                TILT                                       //
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//




function onButtonClicked(element) 
{
	var text = document.getElementById("textEdit1").value;
    websocket.send("M" + text);
}
