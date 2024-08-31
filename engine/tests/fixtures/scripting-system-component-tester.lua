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
function nameGet()
    expectEq(entity.name, "Test name")
end

function nameGetNil()
    expectEq(entity.name, "")
end

function nameSet()
    entity.name = "Hello World"
end

-- Parent
function parentGet()
    expectEq(entity.parent.name, "Parent")
end

function parentGetNil()
    expectEq(entity.parent, nil)
end

function parentSet()
    local parent2 = entityQuery:getFirstEntityByName('Parent 2')
    expectEq(entity.parent.name, 'Parent 1')
    entity.parent = parent2
end

function parentSetNil()
    expectEq(entity.parent.name, 'Parent 1')
    entity.parent = nil
end

-- Local transform 
localTransformPosition = nil

function localTransformPositionGet()
  localTransformPosition = entity.localTransform.position
end

function localTransformPositionSet()
    entity.localTransform.position = Vector3.new(2.5, 3.5, 0.2);
end

function localTransformPositionSetIndividual()
    local position = entity.localTransform.position
    position.x = 2.5
    position.y = 3.5
    position.z = 0.2
end

localTransformScale = nil
function localTransformScaleGet()
    localTransformScale = entity.localTransform.scale
end

function localTransformScaleSet()
    entity.localTransform.scale = Vector3.new(2.5, 3.5, 0.2)
end

function localTransformScaleSetIndividual()
    local scale = entity.localTransform.scale
    scale.x = 2.5
    scale.y = 3.5
    scale.z = 0.2
end

localTransformRotation = nil
function localTransformRotationGet()
    localTransformRotation = entity.localTransform.rotation
end

function localTransformRotationSet()
    entity.localTransform.rotation = Quaternion.fromEulerAngles(0.6108652, 0.4363323, 0.7853982)
end

function localTransformRotationSetIndividual()
    local newRotation = Quaternion.fromEulerAngles(0.6108652, 0.4363323, 0.7853982)

    local rotation = entity.localTransform.rotation
    rotation.x = newRotation.x
    rotation.y = newRotation.y
    rotation.z = newRotation.z
    rotation.w = newRotation.w
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

rigidBodyInertia = false
function rigidBodyGetInertia()
    rigidBodyInertia = entity.rigidBody.inertia
end

function rigidBodySetInertia()
    entity.rigidBody.inertia = Vector3.new(2.5, 3.5, 4.5)
end

function rigidBodySetInertiaIndividual()
    local inertia = entity.rigidBody.inertia
    inertia.x = 2.5
    inertia.y = 3.5
    inertia.z = 4.5
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
sweepEntityName = nil
function collidableSweep()
    sweepOutput, sweepData = entity.collidable:sweep(0.0, 0.0, 0.0, 0.0)

    if sweepOutput then
       sweepNormal = sweepData.normal
       sweepDistance = sweepData.distance
       sweepEntityName = sweepData.entity.name
    end
end

function collidableDelete()
    entity.collidable:delete()
end

-- Text
function textGetTextNil()
    expectEq(entity.text.content, nil)
end

function textGetText()
    expectEq(entity.text.content, "Test content")
end

function textSetText()
    entity.text.content = "Hello World"
end

function textGetLineHeightNil()
    expectEq(entity.text.lineHeight, nil)
end

function textGetLineHeight()
    expectEq(entity.text.lineHeight, 25.0)
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
  local image = ui.image{texture=1}
  expectEq(image.texture, 1)
end

function uiElementImageInvalid()
  local image = ui.image{texture=10}
  expectEq(image.texture, 0)
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
    ui.image{texture=1},
    ui.text{content="Hello"},
    ui.image{texture=2},
    ui.text{content="Test"},
    ui.view{
        children={
            ui.text{content="Child"},
            ui.image{texture=3}
        }
    },
    ui.view{}
  }}

  expectEq(tableLength(view.children), 6)
  expectEq(view.children[1].texture, 1)
  expectEq(view.children[2].content, "Hello")
  expectEq(view.children[3].texture, 2)
  expectEq(view.children[4].content, "Test")
  expectEq(tableLength(view.children[5].children), 2)
  expectEq(view.children[5].children[1].content, "Child")
  expectEq(view.children[5].children[2].texture, 3)
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
        ui.image{texture=1},
        ui.text{content="Hello"},
        ui.image{texture=2},
        ui.text{content="Test"},
        ui.view{
            children={
                ui.text{content="Child"},
                ui.image{texture=3}
            }
        },
        ui.view{}
    }}
    entity.uiCanvas:render(view)
end
