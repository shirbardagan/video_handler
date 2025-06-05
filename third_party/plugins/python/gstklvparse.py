#!/usr/bin/env python3
import logging
import gi
import json

gi.require_version('Gst', '1.0')
gi.require_version('GObject', '2.0')
from gi.repository import Gst, GObject

Gst.init(None)

logger = logging.getLogger(__name__)


def precision_time_stamp_calculator(buff):
    timestamp_int = int.from_bytes(buff, byteorder='big')
    return 'precision_time_stamp', timestamp_int


def platform_heading_angle_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (360 / 65535)
    return 'platform_heading_angle', software_value


def platform_pitch_angle_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (40 / 65534)
    return 'platform_pitch_angle', software_value


def platform_roll_angle_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = (100 / 65534) * int_value
    return 'platform_roll_angle', software_value


def image_source_senor_parser(buff):
    sensor_name = ascii_dec(buff)
    return 'image_source_senor', sensor_name


def mission_id_parser(buff):
    mission_id = ascii_dec(buff)
    return 'mission_id', mission_id


def platform_tail_number_parser(buff):
    platform_tail_number = ascii_dec(buff)
    return 'platform_tail_number', platform_tail_number


def image_coordinate_system_parser(buff):
    c = ascii_dec(buff)
    return 'image_coordinate_system', c


def ascii_dec(byts):
    return byts.decode('ascii', errors='replace')


def decode_ber_oid_tag(data, offset):
    tag = 0
    i = offset
    while True:
        if i >= len(data):
            break
        byte = data[i]
        while byte == 0:
            i += 1
            byte = data[i]
        tag = (tag << 7) | (byte & 0x7F)
        i += 1
        if byte & 0x80 == 0:
            break
    return tag, i


def decode_ber_oid_length(data, offset):
    first_byte = data[offset]
    while first_byte == 0:
        offset += 1
        first_byte = data[offset]
    offset += 1
    if first_byte < 0x80:
        return first_byte, offset
    else:
        num_bytes = first_byte & 0x7f
        length = 0
        for _ in range(num_bytes):
            length = (length << 8) | data[offset]
            offset += 1
        return length, offset


def parse_buffer_to_klv_tuple(data: bytearray, offset: int):
    tag, offset = decode_ber_oid_tag(data, offset)
    if tag == 0:
        return offset, []
    length, offset = decode_ber_oid_length(data, offset)
    value = data[offset: offset + length]
    offset += length
    return offset, [tag, length, value]


def read_klv(klv, buffer):
    offset = 0
    while len(buffer) >= offset:
        offset, tup = parse_buffer_to_klv_tuple(buffer, offset)
        if len(tup) == 0:
            break
        klv.append(tup)
    return klv


def sensor_latitude_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (180 / 4294967294)

    return 'sensor_latitude', software_value


def sensor_longitude_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big', signed=False)
    software_value = int_value * (360 / 4294967294)
    return 'sensor_longitude', software_value


def sensor_true_altitude_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big', signed=False)
    software_value = (19900 / 65535) * int_value - 900
    return 'sensor_true_altitude', software_value


def sensor_horizontal_field_of_view_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (180 / 65535)
    return 'sensor_horizontal_field_of_view', software_value


def sensor_vertical_field_of_view_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (180 / 65535)
    return 'sensor_vertical_field_of_view', software_value


def sensor_relative_azimut_angle_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (360 / 4294967295)
    return 'sensor_relative_azimut_angle', software_value


def sensor_relative_elevation_angle_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (360 / 4294967294)
    return 'sensor_relative_elevation_angle', software_value


def sensor_relative_roll_angle_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (360 / 4294967295)
    return 'sensor_relative_roll_angle', software_value


def slant_range_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (5000000 / 4294967295)
    return 'slant_range', software_value


def icing_detected_parser(buff):
    return 'icing_detected', str(buff)


def target_location_latitude_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (180 / 4294967294)
    return 'target_location_latitude', software_value


def target_location_longitude_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * (360 / 4294967295)
    return 'target_location_longitude', software_value


def target_location_elevation_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = (int_value * (19900 / 65535)) - 900
    return 'target_location_elevation', software_value


def target_track_gate_width_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * 2
    return 'target_track_gate_width', software_value


def target_track_gate_height_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value * 2
    return 'target_track_gate_height', software_value


def platform_angle_of_attack_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = (40 / 65534) * int_value
    return 'platform_angle_of_attack', software_value


def platform_vertical_speed_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = (360 / 65534) * int_value
    return 'platform_vertical_speed', software_value


def airfield_barometric_pressure_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big', signed=False)
    software_value = (5000 / 65535) * int_value
    return 'airfield_barometric_pressure', software_value


def airfield_elevation_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = ((19900 / 65535) * int_value) - 900
    return 'airfield_elevation', software_value


def platform_ground_speed_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value
    return 'platform_ground_speed', software_value


def platform_sideslip_angle_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = (40 / 65534) * int_value
    return 'platform_sideslip_angle', software_value


def platform_fuel_remaining_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = (10000 / 65535) * int_value
    return 'platform_fuel_remaining', software_value


def uas_datalink_LS_version_number_parser(buff):
    int_value = int.from_bytes(buff, byteorder='big')
    software_value = int_value
    return 'uas_datalink_LS_version_number_', software_value


def rvt_local_set_parser(buff):
    rvt_ls_klv = []
    rvt_ls_klv = read_klv(rvt_ls_klv, buff)
    string_value = str(buff)
    return 'rvt_local_set', string_value


def parse_target_series(value, frame_width):
    sub_offset = 0
    targets = []

    while sub_offset + 2 <= len(value):
        target = {}
        id_tag = value[sub_offset]
        sub_offset += 1
        target_id = value[sub_offset]
        sub_offset += 1
        target["id"] = target_id

        while sub_offset + 2 <= len(value):
            sub_tag = value[sub_offset]
            sub_offset += 1
            sub_len = value[sub_offset]
            sub_offset += 1

            if sub_offset + sub_len > len(value):
                break

            sub_val = value[sub_offset:sub_offset + sub_len]
            sub_offset += sub_len

            val = int.from_bytes(sub_val, byteorder='big')
            x = val % frame_width
            y = val // frame_width

            if sub_tag == 2:
                target['top_left'] = [x, y]
            elif sub_tag == 3:
                target['bottom_right'] = [x, y]

            if 'top_left' in target and 'bottom_right' in target:
                targets.append(target)
                break

    return targets


def rvt_vmti_parser(buff):
    offset = 0
    parsed = {}
    targets = []

    while offset < len(buff):
        tag = buff[offset]
        offset += 1

        length_ber, length_bytes_ahead = read_ber_encoded_length(buff[offset:])
        offset += length_ber

        if offset + length_bytes_ahead > len(buff):
            break

        value = buff[offset:offset + length_bytes_ahead]
        offset += length_bytes_ahead

        if tag == 0x03:
            parsed['system_name'] = value.decode('ascii', errors='replace')
        elif tag == 0x04:
            parsed['ls_version'] = value[0]
        elif tag == 0x06:
            parsed['number_reported'] = value[0]
        elif tag == 0x08:
            parsed['frame_width'] = int.from_bytes(value, byteorder='big')
        elif tag == 0x09:
            parsed['frame_height'] = int.from_bytes(value, byteorder='big')
        elif tag == 0x65:
            frame_width = parsed.get('frame_width', 1)
            targets = parse_target_series(value, frame_width)

    if targets:
        parsed['target_series'] = targets

    return 'vmti_local_set', parsed


def checksum_calculator(buff: bytes) -> tuple[str, int]:
    """
    Calculates the checksum of a given byte buffer.

    The checksum is computed by adding each byte (shifted by 0 or 8 bits, alternating)
    to an accumulator, then truncating the result to 16 bits.

    Args:
        buff (bytes): Input buffer for checksum calculation.

    Returns:
        tuple[str, int]: Tuple of the string 'checksum' and the 16-bit checksum value.
    """
    checksum = 0
    length = len(buff)
    for i in range(length):
        shift = 8 * ((i + 1) % 2)
        byte_val = buff[i]
        checksum += byte_val << shift
    return 'checksum', (checksum & 0xFFFF)


def read_ber_encoded_length(buff: bytes) -> tuple[int, int]:
    """
    Reads a BER (Basic Encoding Rules) encoded length field from the buffer.

    Args:
        buff (bytes): Buffer starting with the BER length field.

    Returns:
        tuple[int, int]:
            - Number of bytes read for the length (shift)
            - Decoded length value
    """
    length = buff[0]
    shift = 1
    if length & 0x80 == 0x80:
        masked_value = 0x7f
        read_bytes = length & masked_value
        shift += read_bytes
        length = int.from_bytes(buff[1:read_bytes + 1], byteorder='big')
    return shift, length


tag_function_map = {
    1: checksum_calculator,
    2: precision_time_stamp_calculator,
    3: mission_id_parser,
    4: platform_tail_number_parser,
    5: platform_heading_angle_parser,
    6: platform_pitch_angle_parser,
    7: platform_roll_angle_parser,
    11: image_source_senor_parser,
    12: image_coordinate_system_parser,
    13: sensor_latitude_parser,
    14: sensor_longitude_parser,
    15: sensor_true_altitude_parser,
    16: sensor_horizontal_field_of_view_parser,
    17: sensor_vertical_field_of_view_parser,
    18: sensor_relative_azimut_angle_parser,
    19: sensor_relative_elevation_angle_parser,
    20: sensor_relative_roll_angle_parser,
    21: slant_range_parser,
    34: icing_detected_parser,
    40: target_location_latitude_parser,
    41: target_location_longitude_parser,
    42: target_location_elevation_parser,
    43: target_track_gate_width_parser,
    44: target_track_gate_height_parser,
    50: platform_angle_of_attack_parser,
    51: platform_vertical_speed_parser,
    53: airfield_barometric_pressure_parser,
    54: airfield_elevation_parser,
    56: platform_ground_speed_parser,
    57: platform_sideslip_angle_parser,
    58: platform_fuel_remaining_parser,
    65: uas_datalink_LS_version_number_parser,
    73: rvt_local_set_parser,
    74: rvt_vmti_parser
}


def decode_value_for_key(key_id, encoded_value):
    if key_id in tag_function_map.keys():
        str_key, decoded_value = tag_function_map[key_id](encoded_value)
        return str_key, decoded_value
    else:
        logger.warning("There is no such KLV with tag: %s")


def parse_klv_payload(payload: bytes) -> dict:
    cursor = 0
    parsed_data = {}

    while cursor < len(payload):
        key = payload[cursor]
        cursor += 1

        length_bytes_read, length = read_ber_encoded_length(payload[cursor:])
        cursor += length_bytes_read

        value = payload[cursor: cursor + length]
        cursor += length

        str_key, decoded_value = decode_value_for_key(key, value)
        if str_key:
            parsed_data[str_key] = decoded_value
    return parsed_data


class KLVParse(Gst.Element):
    __gstmetadata__ = (
        "klvparse",
        "Template klvparse",
        "A Parser for KLV data",
        "Elbit Systems"
    )

    __gsttemplates__ = (
        Gst.PadTemplate.new(
            "sink", Gst.PadDirection.SINK, Gst.PadPresence.ALWAYS,
            Gst.Caps.new_any()
        ),
        Gst.PadTemplate.new(
            "src", Gst.PadDirection.SRC, Gst.PadPresence.ALWAYS,
            Gst.Caps.new_any()
        )
    )

    def __init__(self):
        super(KLVParse, self).__init__()
        self.sinkpad = Gst.Pad.new_from_template(self.get_pad_template("sink"), "sink")
        self.srcpad = Gst.Pad.new_from_template(self.get_pad_template("src"), "src")
        self.sinkpad.set_chain_function_full(self.chainfunc)
        self.add_pad(self.sinkpad)
        self.add_pad(self.srcpad)

    def chainfunc(self, pad, parent, buffer):
        success, map_info = buffer.map(Gst.MapFlags.READ)
        try:
            data_bytes = bytes(map_info.data)
            universal_key = data_bytes[0:16]
            length_bytes_read, length = read_ber_encoded_length(data_bytes[16:])
            tag, checksum_value = checksum_calculator(data_bytes[:-2])
            payload = data_bytes[16 + length_bytes_read:-2]
            klv_json = parse_klv_payload(payload)
            klv_json[tag] = checksum_value
            json_string = json.dumps(klv_json)

            out_buffer = Gst.Buffer.new_allocate(None, len(json_string), None)
            out_buffer.fill(0, json_string.encode('utf-8'))

            ret = self.srcpad.push(out_buffer)
            return ret
        except Exception as e:
            logger.error("While parsing klv data: %s", e)
        finally:
            buffer.unmap(map_info)
        return self.srcpad.push(buffer)


GObject.type_register(KLVParse)
__gstelementfactory__ = ("klvparse", Gst.Rank.NONE, KLVParse)
