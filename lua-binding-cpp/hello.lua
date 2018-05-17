player = {
    pos = {
         X = 20,
         Y = 30,
    },
    filename = "res/images/player.png",
    HP = 20,
-- you can also have comments
}

width = 1024
height = 768

function sum(x, y)
  return x + y;
end

local obj = kitten_meta()

print(obj:get())
obj:set("Michan!")
print(obj:get())
