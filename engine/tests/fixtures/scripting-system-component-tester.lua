function expectEq(a, b)
    ret = assertNative(a == b)
    if ret == false then
        msg = "Assertion failed: " .. tostring(a) .. " - " .. tostring(b)
        error(msg)
    end
end

function expectNear(a, b)
  function abs(x)
    if x < 0 then
        return -x
    else
        return x
    end
  end
  ret = assertNative(abs(a - b) < 0.001)
  if ret == false then
      msg = "Assertion failed: " .. tostring(a) .. " - " .. tostring(b)
      error(msg)
  end
end

foundEntity = -1


-- Entity query
local entityQuery = game:get("EntityQuery")

function entityQueryGetFirstByName()
    foundEntity = entityQuery:getFirstEntityByName("Test")
end

function entityQueryDeleteEntity()
    entityQuery:deleteEntity(entity)
end

createdEntity = -1

-- Entity spawner
local entitySpawner = game:get("EntitySpawner")

function entitySpawnerSpawnEmpty()
    createdEntity = entitySpawner:spawnEmpty()
end

function entitySpawnerSpawnPrefab()
    createdEntity = entitySpawner:spawnPrefab(1)
end

function entitySpawnerSpawnSprite()
    createdEntity = entitySpawner:spawnSprite(1)
end

-- Name 
name = ''

function nameGet()
    name = entity.name.value
end

function nameSet()
    entity.name.value = "Hello World"
end

function nameDelete()
    entity.name:delete()
end

-- Local transform 
localPositionX = 0
localPositionY = 0
localPositionZ = 0

function localTransformPositionGet()
    localPositionX, localPositionY, localPositionZ = entity.localTransform:getPosition()
end

function localTransformPositionSet()
    entity.localTransform:setPosition(2.5, 3.5, 0.2)
end

localScaleX = 0
localScaleY = 0
localScaleZ = 0
function localTransformScaleGet()
    localScaleX, localScaleY, localScaleZ = entity.localTransform:getScale()
end

function localTransformScaleSet()
    entity.localTransform:setScale(2.5, 3.5, 0.2)
end

localRotationX = 0
localRotationY = 0
localRotationZ = 0
function localTransformRotationGet()
    localRotationX, localRotationY, localRotationZ = entity.localTransform:getRotation()
end

function localTransformRotationSet()
    entity.localTransform:setRotation(35.0, 25.0, 45.0)
end

function localTransformDelete()
    entity.localTransform:delete()
end

-- Rigid body
function rigidBodyCheckTypeEnumValues()
    expectEq(RigidBodyType.Dynamic, 0)
    expectEq(RigidBodyType.Kinematic, 1)
end

function rigidBodySetDefaultParams()
    entity.rigidBody:setDefaultParams();
end

rbType = nil
function rigidBodyGetType()
  rbType = entity.rigidBody.type
end

mass = 0
function rigidBodyGetMass()
    mass = entity.rigidBody.mass
end

function rigidBodySetMass()
    entity.rigidBody.mass = 2.5
end

inertiaX = 0
inertiaY = 0
inertiaZ = 0
function rigidBodyGetInertia()
    inertiaX, inertiaY, inertiaZ = entity.rigidBody:getInertia()
end

function rigidBodySetInertia()
    entity.rigidBody:setInertia(2.5, 2.5, 2.5)
end

isGravityApplied = true
function rigidBodyIsGravityApplied()
    isGravityApplied = entity.rigidBody.isGravityApplied
end

function rigidBodyApplyGravity()
    entity.rigidBody.isGravityApplied = false
end

function rigidBodyApplyForce()
    entity.rigidBody:applyForce(10.0, 0.2, 5.0);
end

function rigidBodyApplyImpulse()
    entity.rigidBody:applyImpulse(10.0, 0.2, 5.0)
end

function rigidBodyApplyTorque()
    entity.rigidBody:applyTorque(2.5, 3.5, 1.2)
end

function rigidBodyClear()
    entity.rigidBody:clear()
end

function rigidBodyDelete()
    entity.rigidBody:delete()
end

-- Collidable
function collidableSetDefaultMaterial()
    entity.collidable:setDefaultMaterial();
end

staticFriction = 0
function collidableGetStaticFriction()
    staticFriction = entity.collidable.staticFriction
end

function collidableSetStaticFriction()
    entity.collidable.staticFriction = 2.5
end

dynamicFriction = 0
function collidableGetDynamicFriction()
    dynamicFriction = entity.collidable.dynamicFriction
end

function collidableSetDynamicFriction()
    entity.collidable.dynamicFriction = 2.5
end

restitution = 0
function collidableGetRestitution()
    restitution = entity.collidable.restitution
end

function collidableSetRestitution()
    entity.collidable.restitution = 2.5
end

function collidableSetBoxGeometry()
    entity.collidable:setBoxGeometry(2.5, 2.5, 2.5)
end

function collidableSetSphereGeometry()
    entity.collidable:setSphereGeometry(2.5)
end

function collidableSetCapsuleGeometry()
    entity.collidable:setCapsuleGeometry(2.5, 3.5)
end

function collidableSetPlaneGeometry()
    entity.collidable:setPlaneGeometry()
end

sweepOutput = nil
sweepNormal = nil
sweepDistance = nil
sweepData = nil
function collidableSweep()
    sweepOutput, sweepData = entity.collidable:sweep(0.0, 0.0, 0.0, 0.0)

    if sweepOutput then
       sweepNormal = sweepData.normal
       sweepDistance = sweepData.distance
    end
end

function collidableDelete()
    entity.collidable:delete()
end

-- Text
text = ''
textLineHeight = -1

function textGetText()
    text = entity.text.content
end

function textSetText()
    entity.text.content = "Hello World"
end

function textGetLineHeight()
    textLineHeight = entity.text.lineHeight
end

function textSetLineHeight()
    entity.text.lineHeight = 12
end

function textDelete()
    entity.text:delete()
end

-- Animator
function animatorTrigger()
   entity.animator:trigger("Move")
end

function animatorPropertiesValid()
   expectNear(entity.animator.normalizedTime, 0.4)
   expectEq(entity.animator.currentState.name, "StateB")
end

function animatorPropertiesInvalid()
    expectEq(entity.animator.normalizedTime, nil)
    expectEq(entity.animator.currentState, nil)
end

function animatorDelete()
    entity.animator:delete()
end

-- Audio 
audioIsPlayingFlag = false
function audioPlay()
    entity.audio:play()
end

function audioIsPlaying()
    audioIsPlayingFlag = entity.audio.isPlaying
end

function audioDelete()
    entity.audio:delete()
end

-- Perspective lens
prNear = 0
prFar = 0
prSensorWidth = 0
prSensorHeight = 0
prFocalLength = 0
prAperture = 0
prShutterSpeed = 0
prSensitivity = 0

function perspectiveLensGet()
  prNear = entity.perspectiveLens.near
  prFar = entity.perspectiveLens.far
  prSensorWidth, prSensorHeight = entity.perspectiveLens:getSensorSize()
  prFocalLength = entity.perspectiveLens.focalLength
  prAperture = entity.perspectiveLens.aperture
  prShutterSpeed = entity.perspectiveLens.shutterSpeed
  prSensitivity = entity.perspectiveLens.sensitivity
end

function perspectiveLensSet()
    entity.perspectiveLens.near = 0.004
    entity.perspectiveLens.far = 4000.0
    entity.perspectiveLens:setSensorSize(200.0, 200.0)
    entity.perspectiveLens.focalLength = 50.0
    entity.perspectiveLens.aperture = 65.0
    entity.perspectiveLens.shutterSpeed = 2200.0
    entity.perspectiveLens.sensitivity = 4000
end

function perspectiveLensDelete()
    entity.perspectiveLens:delete()
end

-- Input map
inputCommand = 'test'
inputCommandValue = 'test'
inputCommandValueX = 'test'
inputCommandValueY = 'test'

--- getCommand
function inputGetCommand()
    inputCommand = entity.input:getCommand("Test")
end

--- getValueBoolean
function inputGetValueBoolean()
    command = entity.input:getCommand("Test")
    inputCommandValue = entity.input:getValueBoolean(command)
end

function inputGetValueBooleanNonExistentCommand()
    inputCommandValue = entity.input:getValueBoolean(10)
end

function inputGetValueBooleanNoMember()
    command = entity.input:getCommand("Test")
    inputCommandValue = entity.input.getValueBoolean(command)
end

function inputGetValueBooleanNoParam()
    command = entity.input:getCommand()
    inputCommandValue = entity.input:getValueBoolean()
end
 
--- getValueAxis2d
function inputGetValueAxis2d()
    command = entity.input:getCommand("Test")
    inputCommandValueX, inputCommandValueY = entity.input:getValueAxis2d(command)
end

function inputGetValueAxis2dNonExistentCommand()
    inputCommandValueX, inputCommandValueY = entity.input:getValueAxis2d(10)
end
 
--- setScheme
function inputSetScheme()
    entity.input:setScheme("Test scheme")
end

function myComponent()
  return ui.view{
    children={ui.image(10), ui.image(10), ui.image(10)}
  }
end

-- UI canvas
local ui = game:get("UI")

function uiElementImage()
  local image = ui.image{texture=10}
  expectEq(image.texture, 10)
end

function uiElementText()
  local text = ui.text{content="Hello"}
  expectEq(text.content, "Hello")
end

function tableLength(T)
    local count = 0
    for _ in pairs(T) do count = count + 1 end
    return count
  end

function uiElementView()
  local view = ui.view{children={
    ui.image{texture=10},
    ui.text{content="Hello"},
    ui.image{texture=20},
    ui.text{content="Test"},
    ui.view{
        children={
            ui.text{content="Child"},
            ui.image{texture=30}
        }
    },
    ui.view{}
  }}

  expectEq(tableLength(view.children), 6)
  expectEq(view.children[1].texture, 10)
  expectEq(view.children[2].content, "Hello")
  expectEq(view.children[3].texture, 20)
  expectEq(view.children[4].content, "Test")
  expectEq(tableLength(view.children[5].children), 2)
  expectEq(view.children[5].children[1].content, "Child")
  expectEq(view.children[5].children[2].texture, 30)
  expectEq(tableLength(view.children[6].children), 0)
end

function uiElementViewDirection(direction)
    return ui.view{style={direction=direction}}
end

function uiElementViewAlignItems(alignItems)
    return ui.view{style={alignItems=alignItems}}
end

function uiElementViewAlignContent(alignContent)
    return ui.view{style={alignContent=alignContent}}
end

function uiElementViewJustifyContent(justifyContent)
    return ui.view{style={justifyContent=justifyContent}}
end

function uiElementViewFlexGrow(grow)
    return ui.view{style={grow=grow}}
end

function uiElementViewFlexShrink(shrink)
    return ui.view{style={shrink=shrink}}
end

function uiElementViewBackgroundColor(backgroundColor)
    return ui.view{style={backgroundColor=backgroundColor}}
end

function uiCanvasRender()
    view = ui.view{children={
        ui.image{texture=10},
        ui.text{content="Hello"},
        ui.image{texture=20},
        ui.text{content="Test"},
        ui.view{
            children={
                ui.text{content="Child"},
                ui.image{texture=30}
            }
        },
        ui.view{}
    }}
    entity.uiCanvas:render(view)
end
