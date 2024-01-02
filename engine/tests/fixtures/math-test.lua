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

function vector3New()
  local v = Vector3.new(25.0, 35.0, 45.0)

  expectEq(v.x, 25.0)
  expectEq(v.y, 35.0)
  expectEq(v.z, 45.0)
end

function quaternionNew()
  local q = Quaternion.new(0.350225, 0.078362, 0.416451, 0.835327)

  expectNear(q.x, 0.350225)
  expectNear(q.y, 0.078362)
  expectNear(q.z, 0.416451)
  expectNear(q.w, 0.835327)
end

function quaternionFromEulerAngles()
  local q = Quaternion.fromEulerAngles(0.6108652, 0.4363323, 0.7853982)
  
  expectNear(q.x, 0.350225)
  expectNear(q.y, 0.078362)
  expectNear(q.z, 0.416451)
  expectNear(q.w, 0.835327)
end

function quaternionToEulerAngles()
  local q = Quaternion.new(0.350225, 0.078362, 0.416451, 0.835327)
  local v = Quaternion.toEulerAngles(q)

  expectNear(v.x, 0.6108652)
  expectNear(v.y, 0.4363323)
  expectNear(v.z, 0.7853982)
end
