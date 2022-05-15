objectbullet = {}

objectbullet_mt = { __index = objectbullet }

function objectbullet:new(name, tex, x, y, w, h, u, v, nFrames, ang, rad)
    local entity = {}
    setmetatable(entity, objectbullet_mt)
    
    entity.name = name
    entity.texture = tex
    entity.posx = x
    entity.posy = y
    entity.width = w
    entity.height = h
    entity.velx = 0
    entity.vely = 0
    entity.angle = ang
    entity.radius = rad
    entity.alpha = 0
    entity.frames = {}
    entity.currentRow = 0
    entity.currentFrame = 1
    entity.numFrames = nFrames
    entity.thrust = 0
    entity.destroy = false

    return entity
end

function objectbullet:update(dt)
    self.posx = self.posx + 20 * dt * 50

    if self.posx > gameWidth or self.posx < 0 or
        self.posy > gameHeight or self.posy < 0 then
            self.destroy = true --remove it from game
    end
end

function objectbullet:draw()
    love.graphics.draw(self.texture, self.posx,self.posy)
end