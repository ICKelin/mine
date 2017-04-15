// 
var game = Object();
var height_for_grid = 50;
var width_for_grid = 50;
var user = "yingjiu1"

const CMD_MARK = 1
const CMD_OPEN = 2

execute_cmd = function(cmd, x, y, cb) {
	console.log("execute " + cmd + " on (" + x + "," + y + ")");
	var url = "/click?x=" + x + "&y="+y + "&cmd="+cmd + "&user=" + user;
	$.get(url, cb)
}

mark_mine = function(x, y) {
	execute_cmd(CMD_MARK, x, y, function(result) {
		if(result == "game over, you win!") {
			alert("win")
		} else {
			render_scene(result);
		}	
	});
};

open_box = function(x, y) {
	execute_cmd(CMD_OPEN, x, y, function(result) {
		if(result == "position alread marked") {
		}else if(result == "game over, you loss!")
				alert("lose");
		else {
				render_scene(result)
		}
	});
};

render_scene = function(values) {
	var scene = game.scene;
	obj = JSON.parse(values);
	for(var i = 0; i < obj.length; i++) {
		var element = obj[i];
		var x = element.x;
		var y = element.y;
		var value = element.value;
		
		x = (x -1) * width_for_grid;
		y = (y -1) * height_for_grid;
		if (value == -3) { // open, make it white
			scene.fillStyle = "gray";
			scene.fillRect(y + 1, x + 1, height_for_grid - 1, width_for_grid - 1);
		} else if(value == -2) { // mark unknown, mark a ? as text
			xPos = x + width_for_grid / 2;
			yPos = y + height_for_grid / 2;
			scene.fillStyle = "white";
			scene.fillRect(y + 1, x + 1, height_for_grid - 1, width_for_grid - 1);
			scene.fillStyle = "black";
			 scene.fillText("?", yPos, xPos);
		} else if(value == -1) { // mark mine, make it red
			scene.fillStyle = "black";
			scene.fillRect(y + 1, x + 1, height_for_grid - 1, width_for_grid - 1);
		} else if(value == 0) { // as initial, do nothing
			scene.fillStyle = "white";
			scene.fillRect(y + 1, x + 1, height_for_grid - 1, width_for_grid - 1);
		} else { // surrounding mines, mark the value as text
			xPos = x + width_for_grid / 2;
			yPos = y + height_for_grid / 2;
			scene.fillStyle = "black";
			scene.fillText(value, yPos, xPos);
		}
	}
}

click = function(ev) {
		var y = parseInt( (ev.clientX - this.getClientRects()[0].left) / width_for_grid) + 1;
		var x = parseInt( (ev.clientY - this.getClientRects()[0].top) / height_for_grid) + 1;
		// click right mouse
		if (ev.button == 2) {
			mark_mine(x, y);
		} else {
			open_box(x, y);
		}
};

line_to = function(ctx, from_x, from_y, to_x, to_y, color="white") {
	ctx.beginPath();
	ctx.lineWidth = 0.2;
	ctx.strokeStyle = color;
	ctx.moveTo(from_x, from_y);
	ctx.lineTo(to_x, to_y);
	ctx.closePath();
	ctx.stroke();
}

start = function(n, m) {
	document.oncontextmenu = function() {return false;}
	
	game = document.getElementById("game");
	var scene = game.getContext("2d");
	game.style.background = "white"	;

	// initial game scene
	height_for_grid = game.height / n;
	weight_for_grid = game.width / m;
	line_to(scene, 0, 0, game.width, 0, "black");
	line_to(scene, game.width, 0, game.width, game.height, "black");
	for(var i = height_for_grid; i < game.height; i+=height_for_grid) {
		line_to(scene, 0, i, game.width, i, "black");
	}
	for (var i = width_for_grid; i < game.width; i+=width_for_grid) {
		line_to(scene, i, 0, i, game.height, "black");
	}
	
	game.scene = scene;
	game.onmouseup = click

	console.log("initial game");
};

window.onload = function() {
	uri = "/start?n=16&m=16&mine_count=30&user="+user;
	$.get(uri, function(result) {
		start(16, 16);
		render_scene(result);
	})
}
