require "./lib/LiteRGSS.so"
include LiteRGSS

gif = Yuki::GifReader.new(File.dirname(File.absolute_path(__FILE__)) + "/psdk.gif")
host_texture = Bitmap.new(gif.width, gif.height)
gif.update(host_texture)

window = DisplayWindow.new("Test Yuki Gif", 400, 300, 1)
sprite = Sprite.new(window)
sprite.bitmap = host_texture

@running = true
window.on_closed = proc do
  @running = false
end

while @running
	gif.update(host_texture)
	window.update
end
