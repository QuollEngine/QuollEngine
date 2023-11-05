function debug()
    logger.debug("Hello world")
end

function debug_invalid()
    logger.debug()
    logger.debug(nil)
    logger.debug(true)
    logger.debug(debug)
    logger.debug({})
end

function info()
    logger.info("Hello world")
end

function info_invalid()
    logger.info()
    logger.info(nil)
    logger.info(true)
    logger.info(debug)
    logger.info({})
end

function warning()
    logger.warning("Hello world")
end

function warning_invalid()
    logger.warning()
    logger.warning(nil)
    logger.warning(true)
    logger.warning(debug)
    logger.warning({})
end

function error()
    logger.error("Hello world")
end

function error_invalid()
    logger.error()
    logger.error(nil)
    logger.error(true)
    logger.error(debug)
    logger.error({})
end

function fatal()
    logger.fatal("Hello world")
end

function fatal_invalid()
    logger.fatal()
    logger.fatal(nil)
    logger.fatal(true)
    logger.fatal(debug)
    logger.fatal({})
end
