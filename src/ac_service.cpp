#include "service/ac_controller.hpp"
#include "service/ac_receiver.hpp"

AcControllerService acControl(PIN_IR_LED);
AcReceiverService acReceive(PIN_IR_RECEIVER);

decode_results results;