local logger = game:get('Logger')

function debug()
    logger.debug("Hello world")
end

function debugInvalid()
    logger.debug()
    logger.debug(nil)
    logger.debug(true)
    logger.debug(debug)
    logger.debug({})
end

function info()
    logger.info("Hello world")
end

function infoInvalid()
    logger.info()
    logger.info(nil)
    logger.info(true)
    logger.info(debug)
    logger.info({})
end

function warning()
    logger.warning("Hello world")
end

function warningInvalid()
    logger.warning()
    logger.warning(nil)
    logger.warning(true)
    logger.warning(debug)
    logger.warning({})
end

function error()
    logger.error("Hello world")
end

function errorInvalid()
    logger.error()
    logger.error(nil)
    logger.error(true)
    logger.error(debug)
    logger.error({})
end

function fatal()
    logger.fatal("Hello world")
end

function fatalInvalid()
    logger.fatal()
    logger.fatal(nil)
    logger.fatal(true)
    logger.fatal(debug)
    logger.fatal({})
end
