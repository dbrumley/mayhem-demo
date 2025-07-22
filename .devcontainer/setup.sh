#!/bin/bash
set -e

if [ -n "$MAYHEM_TOKEN" ]; then
  mayhem login --token "$MAYHEM_TOKEN"
  echo "🔑 Logged into Mayhem CLI automatically."
else
  echo "⚠️ MAYHEM_TOKEN not set. Run 'mayhem login' manually."
fi