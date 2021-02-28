require "./lib/LiteRGSS.so"
include LiteRGSS

game_title = "Test Window Settings"
width = 320
height = 0
scale = 2
bpp = 32
n = 60
vsync = false
fullscreen = false
show_mouse = false

window = DisplayWindow.new(game_title, width, height, scale, bpp, n, vsync, fullscreen, show_mouse);

puts "----- Init -----"
puts window.settings

settings = window.settings

# Vsync ON
settings[6] = true

# Fullscreen ON
settings[7] = true

window.settings = settings

puts "----- 1 -----"
puts window.settings

sleep 3

# Fullscreen OFF
settings[7] = false

# Resize : Width
settings[1] = 400

window.settings = settings

puts "----- 2 -----"
puts window.settings

sleep 3

# Edit window title
settings[0] = "Last step èé !"

# Resize : Height
settings[2] = 300

window.settings = settings

puts "----- 3 -----"
puts window.settings

sleep 3
