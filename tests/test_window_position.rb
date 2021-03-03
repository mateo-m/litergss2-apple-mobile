require "./lib/LiteRGSS.so"
include LiteRGSS

window = DisplayWindow.new("Test Window Position", 100, 100, 2);

window.x = DisplayWindow::desktop_width / 2 - 100
window.y = DisplayWindow::desktop_height / 2 - 100

puts "#{window.x}; #{window.y}"

sleep 3

window.x += 10

sleep 3

window.y += 10

sleep 3

puts "#{window.x}; #{window.y}"
