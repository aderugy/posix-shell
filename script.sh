for cmd in ls nonexistentcommand pwd; do $cmd || echo "Error with $cmd"; done
