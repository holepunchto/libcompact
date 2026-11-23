# TODO

Agent task tracking for this repo. One entry per line:

```markdown
- [ ] **Title** `[role]` - one sentence saying what the work is. (origin)
```

`[role]` is one of `needs-triage`, `needs-info`, `ready-for-agent`, `ready-for-human`, `wontfix`. Sub-items and findings go in indented bullets beneath an entry. The PR that finishes an entry deletes it; git history is the record of what shipped.

- [ ] **`utf8` decode returns invalid bytes unchanged** `[needs-triage]` - CODECS.md substitutes U+FFFD by maximal subpart, `compact_decode_utf8` hands back a view into the buffer, so the same wire bytes decode to a different value here than in the reference. (conformance run against compact-encoding-test 0.3.1)
  - the nine cases are `utf8-invalid-bytes`, `utf8-overlong`, `utf8-overlong-four-byte`, `utf8-encoded-surrogate`, `utf8-above-max-code-point`, `utf8-subpart-completable`, `utf8-subpart-uncompletable`, `utf8-stranded-continuations`, `utf8-count-cuts-a-sequence-short`
  - substitution changes the length, so it cannot be done in a view into the decoded buffer; whether to pay for a copy is the decision this entry waits on
