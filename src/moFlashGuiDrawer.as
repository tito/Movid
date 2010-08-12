package {
	/*
	 *
	 * TODO:
	 *	* Support rest of the drawing instructions (line, ...).
	 *	* Get refreshDelay from pipeline update speed.
	 *
	 */
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.events.IOErrorEvent;
	import flash.events.ErrorEvent;
	import flash.events.MouseEvent;
	import flash.utils.setTimeout;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.display.StageScaleMode;
	import flash.display.StageAlign;
	import flash.display.StageDisplayState;
	import flash.display.LoaderInfo;
	import flash.text.TextField;

	public class moFlashGuiDrawer extends Sprite {
		private var request:URLRequest;
		private var loader:URLLoader;
		private var instructions:String;
		private var refreshDelay:Number;
		private var drawing:Sprite;
		private var viewport_width:int;
		private var viewport_height:int;
		private var fullscreenbtn:Sprite;

		public function moFlashGuiDrawer() {
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			this.stage.addEventListener(Event.RESIZE, this.handle_stage_resize);
			drawing = new Sprite();
			addChild(drawing);
			refreshDelay = 100;
			// Movid tells us where to load the drawing instructions from (via FlashVars):
			var params:Object = LoaderInfo(this.root.loaderInfo).parameters;
			var url:String = "http://" + params["ip"] + ":" + params["port"] +
					 "/pipeline/gui?objectname=" + params["objectname"];
			request = new URLRequest(url);
			loader = new URLLoader();
			loader.addEventListener(Event.COMPLETE, request_complete);
			loader.addEventListener(IOErrorEvent.IO_ERROR, function(e:IOErrorEvent):void {
							setTimeout(perform_request, refreshDelay);
						});
			this.perform_request();

			// Flash plugin (fortunately) doesn't allow to launch in fullscreen directly.
			// Unfortunately that means we need an extra button to go fullscreen, tho.
			fullscreenbtn = new Sprite();
			fullscreenbtn.graphics.beginFill(0xCC6600, 0.7);
			fullscreenbtn.graphics.drawRoundRect(0, 0, 80, 25, 10, 10);
			fullscreenbtn.graphics.endFill();
			fullscreenbtn.buttonMode = true;
			fullscreenbtn.addEventListener(MouseEvent.CLICK, go_fullscreen);
			var label:TextField = new TextField()
			label.text = "Fullscreen";
			label.x = 10;
			label.y = 5;
			label.selectable = false;
			fullscreenbtn.addChild(label)

			addChild(fullscreenbtn);
		}

		public function handle_stage_resize(e:Event):void {
			// We're leaving fullscreen mode. Add button back.
			addChild(fullscreenbtn);
		}

		public function go_fullscreen(event:MouseEvent):void {
			var screenRectangle:Rectangle = new Rectangle(0, 0, stage.fullScreenWidth, stage.fullScreenHeight);
			stage.fullScreenSourceRect = screenRectangle;
			stage.displayState = StageDisplayState.FULL_SCREEN;
			removeChild(fullscreenbtn);
		}

		public function perform_request():void {
			loader.load(request);
		}

		public function rgbToHex(r:int, g:int, b:int):int {
			return r<<16 | g<<8 | b;
		}

		public function transform_coords(x:Number, y:Number):Point {
			var ww:int = stage.stageWidth;
			var wh:int = stage.stageHeight;
			// Both movid and flash have their origin in the upper left corner.
			x = x / viewport_width * ww;
			y = y / viewport_height * wh;
			return new Point(x, y);
		}

		public function request_complete(evt:Event):void {
			instructions = loader.data;
			// Parse instructions & draw
			var commands:Array = instructions.split("\n");
			drawing.graphics.clear();
			for each (var command:String in commands) {
				var args:Array = command.split(" ");
				switch (args[0]) {
					case "line":
						var x1:Number = Number(args[1]);
						var y1:Number = Number(args[2]);
						var start:Point = transform_coords(x1, y1);
						var x2:Number = Number(args[3]);
						var y2:Number = Number(args[4]);
						var end:Point = transform_coords(x2, y2);
						drawing.graphics.moveTo(start.x, start.y);
						drawing.graphics.lineTo(end.x, end.y);
						break;
					case "circle":
						var x:Number = Number(args[1]);
						var y:Number = Number(args[2]);
						var radius:Number = Number(args[3]);
						var trans:Point = transform_coords(x, y);
						drawing.graphics.drawCircle(trans.x, trans.y, radius);
						break;
					case "color":
						var r:int = int(args[1]);
						var g:int = int(args[2]);
						var b:int = int(args[3]);
						var hex:int = rgbToHex(r, g, b);
						drawing.graphics.beginFill(hex, 1)
						drawing.graphics.lineStyle(1, hex, 1.0);
						break;
					case "viewport":
						viewport_width = int(args[1]);
						viewport_height = int(args[2]);
						break;
				}
			}
			drawing.graphics.endFill();
			setTimeout(perform_request, refreshDelay);
		}
	}
}
