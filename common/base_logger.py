import logging

def configure_logging(level=logging.DEBUG):
    if not logging.getLogger().hasHandlers():
        logging.basicConfig(
            level=level,
            format="%(levelname)s - %(message)s",
        )

configure_logging()
logger = logging.getLogger(__name__)