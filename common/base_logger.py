import logging
import colorlog

def configure_logging(level=logging.DEBUG):
    formatter = colorlog.ColoredFormatter(
        "%(log_color)s%(levelname)s - %(message)s",
        log_colors={
            "DEBUG": "cyan",
            "INFO": "green",
            "WARNING": "yellow",
            "ERROR": "red",
            "CRITICAL": "bold_red",
        },
    )

    handler = logging.StreamHandler()
    handler.setFormatter(formatter)

    logger = logging.getLogger()
    logger.setLevel(level)
    logger.addHandler(handler)
    logger.propagate = False

configure_logging()
logger = logging.getLogger(__name__)
