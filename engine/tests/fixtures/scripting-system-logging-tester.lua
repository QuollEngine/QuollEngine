local logger = game:get('Logger')

function someFunc()
end

function stdPrint()
    print("Hello world", 12, true, nil, someFunc, {hello="world"})
end

function debug()
    logger.debug("Hello world", 12, true, nil, someFunc, {hello="world"})
end

function info()
    logger.info("Hello world", 12, true, nil, someFunc, {hello="world"})
end

function warning()
    logger.warning("Hello world", 12, true, nil, someFunc, {hello="world"})
end

function error()
    logger.error("Hello world", 12, true, nil, someFunc, {hello="world"})
end

function fatal()
    logger.fatal("Hello world", 12, true, nil, someFunc, {hello="world"})
end

