#pragma once

#include <string_view>

static constexpr std::string_view DEFAULT_CONFIG_STR = R"EOF(rules:
- description: IPv4
  regex: \b((?:25[0-5]|2[0-4][0-9]|1?[0-9]{1,2})(?:\.(?:25[0-5]|2[0-4][0-9]|1?[0-9]{1,2})){3})\b
  color: f#FF4DFF

- description: MAC address
  regex: \b([0-9A-Fa-f]{2}(?::[0-9A-Fa-f]{2}){5})\b
  color: f#FF4DFF

- description: Error
  regex: (?i)\b(password|abnormal(ly)?|down|los(t|ing)|err(ors?)?|(den(y|ies|ied)?)|reject(ing|ed)?|drop(ped|s)?|(err\-)?disabled?|(timed?\-?out)|fail(s|ed|iure)?|disconnect(ed)?|unreachable|invalid|bad|notconnect|unusable|block(ed|ing)?|blk|inaccessible|wrong|collisions?|unsynchronized|mismatch|runts)\b
  color: f#FF6060

- description: Warning
  regex: (?i)\b(warn(ing)?s?|no(pe)?|exit(ed)?|reset(t?ing)?|discard(ed|ing)?|filter(ed|ing)?|stop(p(ed|ing))?|never|bad|can((')?t|not))\b
  color: f#DEDC12

- description: Success
  regex: (?i)\b(ye(s|ah?|p)?|start(ed|ing)?|running|good|up|ok(ay)?|permit(ed|s)?|accept(s|ed)?|enabled?|online|succe((ss(ful|fully)?)|ed(ed)?)?|connect(ed)?|reachable|valid|forwarding|synchronized)\b
  color: f#18ED93
)EOF";
