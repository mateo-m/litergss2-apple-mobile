require "./lib/LiteRGSS.so"
include LiteRGSS

w = 800
h = 600
window = DisplayWindow.new("Test Viewport Zoom", w, h, 1, 32, 20, false, false, true);
v = Viewport.new(window, 0, 0, w, h)
v.ox = -100
v.oy = -100
image = Sprite.new(v)
image.bitmap = Bitmap.new(File.dirname(File.absolute_path(__FILE__)) + "/image.png")

puts "Step 0"

window.update
sleep 3

puts "Step 1"
v.zoom = 0.5
window.update
sleep 3

puts "Step 2"

v.zoom = 0.25
window.update
sleep 3

puts "Step 3"

v.zoom = 0.25
window.update
sleep 3

puts "Step 4"

v.zoom = 2
window.update
sleep 3