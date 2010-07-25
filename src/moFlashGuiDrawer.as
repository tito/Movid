package {
	/*
	 *
	 * TODO:
	 *	* Support rest of the drawing instructions (line, ...).
	 *	* Make fullscreen work.
	 *		Apparently needs an extra button to trigger it due to security reasons
	 *	* Get refreshDelay from pipeline update speed.
	 *	* Integrate with Movid (may need to accept some additional args for that)
	 *		Get current URL: trace("loaderurl " + loaderInfo.loaderURL);
	 *
	 */
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.events.IOErrorEvent;
	import flash.events.ErrorEvent;
	import flash.utils.setTimeout;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.display.StageScaleMode;
	import flash.display.StageAlign;
	import flash.display.StageDisplayState;

	public class moFlashGuiDrawer extends Sprite {
		private var request:URLRequest;
		private var loader:URLLoader;
		private var instructions:String;
		private var refreshDelay:Number;
		private var drawing:Sprite;
		private var viewport_width:int;
		private var viewport_height:int;

		public function moFlashGuiDrawer() {
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			//var screenRectangle:Rectangle = new Rectangle(0, 0, stage.stageWidth, stage.stageHeight);
			//var screenRectangle:Rectangle = new Rectangle(0, 0, stage.fullScreenWidth, stage.fullScreenHeight);
			//stage.fullScreenSourceRect = screenRectangle;
			//stage.displayState = StageDisplayState.FULL_SCREEN;
			drawing = new Sprite();
			addChild(drawing);
			refreshDelay = 100;
			request = new URLRequest("http://127.0.0.1:7500/pipeline/gui?objectname=calib");
			loader = new URLLoader();
			loader.addEventListener(Event.COMPLETE, request_complete);
			loader.addEventListener(IOErrorEvent.IO_ERROR, function(e:IOErrorEvent):void {
							setTimeout(perform_request, refreshDelay);
						});
			this.perform_request();
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
						drawing.graphics.beginFill(rgbToHex(r, g, b), 1)
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
