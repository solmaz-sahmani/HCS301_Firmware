/*
 * Application-side integration example.
 *
 * The real application remains responsible for:
 *   - device_config
 *   - hcs301_protocol
 *   - remote_storage
 *   - LED indication
 *
 * The hardware RF layer only returns the 66 received bits.
 *
 * Example:
 *
 * uint8_t bits[66];
 * hcs301_frame_t frame;
 *
 * if (rf_driver_stm32_receive(bits))
 * {
 *     status_t status;
 *
 *     status = hcs301_protocol_decode(
 *         hcs301_protocol_get_instance(),
 *         bits,
 *         key,
 *         &frame);
 *
 *     if (status == STATUS_OK)
 *     {
 *         status = hcs301_protocol_verify(
 *             hcs301_protocol_get_instance(),
 *             &frame,
 *             serial,
 *             discrimination);
 *     }
 *
 *     if (status == STATUS_OK)
 *     {
 *         // Valid remote command.
 *     }
 * }
 */
