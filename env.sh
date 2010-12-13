# .------------------------------------------.
# | Setup the development environment        |
# '------------------------------------------'

tc="$(cd $(pwd)/../tachyon2-toolchain/installed/bin; pwd)"
tcs="$(cd $(pwd)/../tachyon2-toolchain/installed/sbin; pwd)"

#otc="$(cd $(pwd)/../tachyon2-toolchain/optional/installed/bin; pwd)"

if [[ -d ${tc} ]]; then
    echo "using toolchain in ${tc}..."
    export PATH="${tc}:${tcs}:${PATH}"
fi
